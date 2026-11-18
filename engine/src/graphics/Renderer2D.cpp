#include <forge2d/graphics/Renderer2D.hpp>

#include <cmath>
#include <forge2d/debug/Assert.hpp>
#include <forge2d/graphics/Camera2D.hpp>
#include <forge2d/graphics/Texture2D.hpp>
#include <graphics/internal/BatchBuilder2D.hpp>
#include <graphics/internal/RenderView2D.hpp>
#include <graphics/internal/TextureAccess.hpp>
#include <graphics/internal/TextRenderer.hpp>
#include <opengl/OpenGLRenderer2D.hpp>

namespace forge
{
    struct Renderer2D::Impl
    {
        using Backend = internal::opengl::OpenGLRenderer2D;

        Backend backend;
        internal::BatchBuilder2D batches;
        internal::TextRenderer text;

        math::Vec2             logicalSize{};
        internal::RenderView2D defaultView{};
        internal::RenderView2D currentView{};

        bool active   = false;
        bool drawable = false;

        explicit Impl(const std::filesystem::path& shaders)
        : backend(shaders),
          batches(backend.whiteTexture(), Backend::MaxVertices, Backend::MaxIndices, Backend::MaxTextures),
          text(backend) { }

        bool canDraw() const
        {
            FORGE_ASSERT(active, "Drawing requires beginFrame()");
            return active && drawable;
        }

        void flush()
        {
            if (batches.batch().empty())
            {
                return;
            }

            backend.submit(batches.batch(), currentView);
            batches.clear();
        }

        template<typename Append>
        void append(Append&& operation)
        {
            if (!canDraw()) return;
            if (!operation())
            {
                flush();
                FORGE_CHECK(operation(), "Primitive cannot fit in an empty 2D batch");
            }
        }

        void selectView(internal::RenderView2D view)
        {
            if (!canDraw())
            {
                return;
            }

            flush();
            currentView = view;
        }
    };

    Renderer2D::Renderer2D(const std::filesystem::path& shaderDirectory) : impl_(std::make_unique<Impl>(shaderDirectory)) { }
    Renderer2D::~Renderer2D() = default;

    void Renderer2D::beginFrame(const FrameInfo& frame, const Colour& clearColour)
    {
        FORGE_ASSERT(!impl_->active, "Previous frame must end before beginFrame()");

        impl_->active = true;
        impl_->batches.clear();
        impl_->text.beginFrame();
        impl_->logicalSize = frame.logicalSize;

        impl_->drawable = std::isfinite(frame.logicalSize.x) && std::isfinite(frame.logicalSize.y) &&
                          frame.logicalSize.x > 0    && frame.logicalSize.y > 0                    &&
                          frame.framebufferWidth > 0 && frame.framebufferHeight > 0;

        if (!impl_->drawable) return;

        impl_->defaultView = internal::RenderView2D::screen(frame.logicalSize);
        impl_->currentView = impl_->defaultView;

        impl_->backend.beginFrame(frame.framebufferWidth, frame.framebufferHeight, clearColour);
    }

    void Renderer2D::endFrame()
    {
        FORGE_ASSERT(impl_->active, "endFrame() requires beginFrame()");
        if (!impl_->active) return;
        if (impl_->drawable)
        {
            impl_->flush();
            impl_->backend.endFrame();
        }
        impl_->active = false;
        impl_->drawable = false;
    }

    void Renderer2D::setCamera(const Camera2D& camera)
    {
        if (impl_->canDraw())
            impl_->selectView({camera.viewProjection(impl_->logicalSize)});
    }

    void Renderer2D::resetCamera()
    {
        impl_->selectView(impl_->defaultView);
    }

    void Renderer2D::drawTriangle(math::Transform2D transform, Colour colour, bool fill)
    {
        impl_->append([&] { return impl_->batches.tryAppendTriangle(transform, colour, fill); });
    }

    void Renderer2D::drawRect(math::Transform2D transform, Colour colour, bool fill)
    {
        impl_->append([&] { return impl_->batches.tryAppendRect(transform, colour, fill); });
    }

    void Renderer2D::drawSprite(const Texture2D& texture, math::Transform2D transform, Colour tint)
    {
        if (!FORGE_ENSURE(texture.valid(), "drawSprite called with an empty Texture2D"))
        {
            return;
        }

        const auto resource = internal::TextureAccess::resource(texture);
        impl_->append([&] { return impl_->batches.tryAppendSprite(resource, transform, tint); });
    }

    void Renderer2D::drawLine(math::Vec2 from, math::Vec2 to, Colour colour, float thickness)
    {
        impl_->append([&] { return impl_->batches.tryAppendLine(from, to, colour, thickness); });
    }

    void Renderer2D::drawCircle(math::Vec2 centre, float radius, Colour colour, bool fill)
    {
        impl_->append([&] { return impl_->batches.tryAppendCircle(centre, radius, colour, fill); });
    }

    void Renderer2D::drawText(std::string_view text, const Font& font, math::Vec2 position, float size, Colour colour)
    {
        if (!impl_->canDraw() || !font.valid() || text.empty())
        {
            return;
        }

        impl_->text.draw(text, font, position, size, colour, impl_->batches, [&] { impl_->flush(); });
    }

    void Renderer2D::drawText(std::string_view text, const Font* font, math::Vec2 position,float size, Colour colour)
    {
        if (font)
        {
            drawText(text, *font, position, size, colour);
        }
    }
}
