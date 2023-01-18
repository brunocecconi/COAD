
#if OPENGL_ENABLED

LOG_DEFINE(OpenglManager);

namespace Render::Platform
{

class OpenglManager
{
	CLASS_BODY_NON_MOVEABLE_COPYABLE(OpenglManager);

	friend class Manager;

private:
	OpenglManager();

public:
	~OpenglManager();

private:
	void Initialize(RESULT_PARAM_DEFINE);
	void Update(RESULT_PARAM_DEFINE);
	void Finalize(RESULT_PARAM_DEFINE);
	void Resize(glm::uvec2 NewSize, RESULT_PARAM_DEFINE);
	void MakeCurrentContext(RESULT_PARAM_DEFINE) const;
	void ResetCurrentContext(RESULT_PARAM_DEFINE) const;

private:
	bool mVsync = true;
#if EDITOR
	bool mEditor = true;
#endif
#if PLATFORM_WINDOWS
	HDC mDc{};
	HGLRC				  mContext{};
	PIXELFORMATDESCRIPTOR mPixelFormatDesc{};
#endif

	Engine::Window& mWindow;
};

INLINE OpenglManager::OpenglManager() : mWindow{Engine::Manager::Instance().GetWindow()}
{
}

INLINE OpenglManager::~OpenglManager()
{
}

INLINE void OpenglManager::Initialize(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

#if PLATFORM_WINDOWS
	mPixelFormatDesc = {sizeof(PIXELFORMATDESCRIPTOR),
						1,
						PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // Flags
						PFD_TYPE_RGBA, // The kind of framebuffer. RGBA or palette.
						32,			   // Colordepth of the framebuffer.
						0,
						0,
						0,
						0,
						0,
						0,
						0,
						0,
						0,
						0,
						0,
						0,
						0,
						24, // Number of bits for the depthbuffer
						8,	// Number of bits for the stencilbuffer
						0,	// Number of Aux buffers in the framebuffer.
						PFD_MAIN_PLANE,
						0,
						0,
						0,
						0};

	mDc = GetDC(mWindow.GetHandle());
	const auto lPixelFormat = ChoosePixelFormat(mDc, &mPixelFormatDesc);
	SetPixelFormat(mDc, lPixelFormat, &mPixelFormatDesc);
	mContext = wglCreateContext(mDc);
	wglMakeCurrent(nullptr, nullptr);
#endif

	RESULT_OK();
}

INLINE void OpenglManager::Update(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	Editor::Instance().RunExternal();

	ImGui::Render();
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SwapBuffers(mDc);

	RESULT_OK();
}

INLINE void OpenglManager::Finalize(RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();

#if PLATFORM_WINDOWS
	ReleaseDC(mWindow.GetHandle(), mDc);
	wglDeleteContext(mContext);
#endif

	RESULT_OK();
}

INLINE void OpenglManager::Resize(const glm::uvec2 NewSize, RESULT_PARAM_IMPL)
{
	RESULT_ENSURE_LAST();
	glViewport(0, 0, NewSize.x, NewSize.y);
	const auto lWindowSize = mWindow.GetPreviousSize();
	LOGC(Info, OpenglManager, "Resize viewport [%u,%u] to [%u,%u].", lWindowSize.x,lWindowSize.y,NewSize.x,NewSize.y);
	RESULT_OK();
}

INLINE void OpenglManager::MakeCurrentContext(RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(mContext, NullPtr);
#if PLATFORM_WINDOWS
	wglMakeCurrent(mDc, mContext);
	glewExperimental = true;
	RESULT_CONDITION_ENSURE(glewInit() == GLEW_OK, OpenglFailedToInitializeGlew);

	glViewport(0,0,mWindow.GetSize().x, mWindow.GetSize().y);

	RESULT_CONDITION_ENSURE(GLEW_EXT_framebuffer_object, OpenglFailedToRequiredExtensionIsNotSupported);

	LOGC(Verbose, OpenglManager, "OpenGL version: %s", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	LOGC(Verbose, OpenglManager, "GLEW version: %s", reinterpret_cast<const char*>(glewGetString(GLEW_VERSION)));
#endif
	RESULT_OK();
}

INLINE void OpenglManager::ResetCurrentContext(RESULT_PARAM_IMPL) const
{
	RESULT_ENSURE_LAST();
	RESULT_CONDITION_ENSURE(mContext, NullPtr);
#if PLATFORM_WINDOWS
	wglMakeCurrent(nullptr, nullptr);
#endif
	RESULT_OK();
}

} // namespace Render::Platform

CLASS_VALIDATION(Render::Platform::OpenglManager);
#else
namespace Render::Platform
{
class OpenglManager{};
}
#endif