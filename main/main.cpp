#include <glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <typeinfo>
#include <stdexcept>

#include <cstdio>
#include <cstdlib>

#include "../support/error.hpp"
#include "../support/program.hpp"
#include "../support/checkpoint.hpp"
#include "../support/debug_output.hpp"
#include "rapidobj/rapidobj.hpp"


#include "defaults.hpp"


#include "camera.h"
#include "Render.h"


namespace
{
	//
	//void glfw_callback_error_( int, char const* );

	//void glfw_callback_key_( GLFWwindow*, int, int, int, int );

	struct GLFWCleanupHelper
	{
		~GLFWCleanupHelper();
	};
	struct GLFWWindowDeleter
	{
		~GLFWWindowDeleter();
		GLFWwindow* window;
	};
}

namespace WindowControl
{
	constexpr char const* kWindowTitle = "COMP3811 - Coursework 2";

	int _window_height_;
	int _window_width_;


	float lastX;
	float lastY;
	bool firstMouse;
	float deltaTime;
	float lastFrameTime;
	Camera camera(Vec3f{ 0.f, 1.f, 5.f });

	void glfw_callback_error_( int aErrNum, char const* aErrDesc )
	{
		std::fprintf( stderr, "GLFW error: %s (%d)\n", aErrDesc, aErrNum );
	}

	void glfw_callback_key_( GLFWwindow* aWindow, int aKey, int, int aAction, int )
	{
		if( GLFW_KEY_ESCAPE == aKey && GLFW_PRESS == aAction )
		{
			glfwSetWindowShouldClose( aWindow, GLFW_TRUE );
			return;
		}
		if (glfwGetKey(aWindow, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(aWindow, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(aWindow, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(aWindow, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);
		if (glfwGetKey(aWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			camera.MovementSpeed *= 1.2;
		if (glfwGetKey(aWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			camera.MovementSpeed *= 0.8;
	}


	void glfw_callback_mouse_(GLFWwindow* window, double xpos, double ypos)
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		{
			lastX = xpos;
			lastY = ypos;
			return;
		}
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);

	}


	void glfw_framebuffer_size_(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}
}

namespace
{
	GLFWCleanupHelper::~GLFWCleanupHelper()
	{
		glfwTerminate();
	}

	GLFWWindowDeleter::~GLFWWindowDeleter()
	{
		if( window )
			glfwDestroyWindow( window );
	}
}



namespace SceneControl
{
	Mat44f matrix_view;
	Mat44f matrix_projection;
	std::shared_ptr<RenderObject> cube;
	std::shared_ptr<RenderObject> cat;
	std::shared_ptr<Shader> shader_phong;
	std::vector<Model> scene;

	std::shared_ptr<PhongMaterial> material_base, material_s, material_d, material_e;
	std::shared_ptr<Light> light_main;
	unsigned int texture_base;
	unsigned int texture_cat;

	std::shared_ptr<RenderObject> set_cube_ro() {
		std::vector<unsigned int> indices = {
		0,1,3, // x -ve
		3,2,0, // x -ve
		4,6,7, // x +ve
		7,5,4, // x +ve
		9,13,15, // z +ve
		15,11,9, // z +ve
		8,10,14, // z -ve
		14,12,8, // z -ve
		16,20,21, // y -ve
		21,17,16, // y -ve
		23,22,18, // y +ve
		18,19,23  // y +ve
		};
		std::vector<Vertex> vertices = {
			{ -0.5f, -0.5f, -0.5f,  -1, 0, 0,  0.0f, 0.0f},  // 0
			{ -0.5f, -0.5f,  0.5f,  -1, 0, 0,  1.0f, 0.0f},  // 1
			{ -0.5f,  0.5f, -0.5f,  -1, 0, 0,  0.0f, 1.0f},  // 2
			{ -0.5f,  0.5f,  0.5f,  -1, 0, 0,  1.0f, 1.0f},  // 3
			{0.5f, -0.5f, -0.5f,   1, 0, 0,  1.0f, 0.0f},  // 4
			{0.5f, -0.5f,  0.5f,   1, 0, 0,  0.0f, 0.0f},  // 5
			{0.5f,  0.5f, -0.5f,   1, 0, 0,  1.0f, 1.0f},  // 6
			{0.5f,  0.5f,  0.5f,   1, 0, 0,  0.0f, 1.0f},  // 7

			{-0.5f, -0.5f, -0.5f,  0,0,-1,  1.0f, 0.0f},  // 8
			{-0.5f, -0.5f,  0.5f,  0,0,1,   0.0f, 0.0f},  // 9
			{-0.5f,  0.5f, -0.5f,  0,0,-1,  1.0f, 1.0f},  // 10
			{-0.5f,  0.5f,  0.5f,  0,0,1,   0.0f, 1.0f},  // 11
			{0.5f, -0.5f, -0.5f,  0,0,-1,  0.0f, 0.0f},  // 12
			{0.5f, -0.5f,  0.5f,  0,0,1,   1.0f, 0.0f},  // 13
			{0.5f,  0.5f, -0.5f,  0,0,-1,  0.0f, 1.0f},  // 14
			{0.5f,  0.5f,  0.5f,  0,0,1,   1.0f, 1.0f},  // 15

			{-0.5f, -0.5f, -0.5f,  0,-1,0,  0.0f, 0.0f},  // 16
			{-0.5f, -0.5f,  0.5f,  0,-1,0,  0.0f, 1.0f},  // 17
			{-0.5f,  0.5f, -0.5f,  0,1,0,   0.0f, 1.0f},  // 18
			{-0.5f,  0.5f,  0.5f,  0,1,0,   0.0f, 0.0f},  // 19
			{0.5f, -0.5f, -0.5f,  0,-1,0,  1.0f, 0.0f},  // 20
			{0.5f, -0.5f,  0.5f,  0,-1,0,  1.0f, 1.0f},  // 21
			{0.5f,  0.5f, -0.5f,  0,1,0,   1.0f, 1.0f},  // 22
			{0.5f,  0.5f,  0.5f,  0,1,0,   1.0f, 0.0f}   // 23
		};

		return std::make_shared<RenderObject>(vertices, indices);
	}

	std::shared_ptr<RenderObject> set_obj_ro(const char* path) {
		rapidobj::Result result = rapidobj::ParseFile(path);

		if (result.error) {
			std::cout << result.error.code.message() << '\n';
			return nullptr;
		}

		bool success = rapidobj::Triangulate(result);

		if (!success) {
			std::cout << result.error.code.message() << '\n';
			return nullptr;
		}

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		for (const auto& face : result.shapes) {
			for (const auto& id : face.mesh.indices) {
				indices.push_back(indices.size());
				vertices.push_back(
					Vertex{
					result.attributes.positions[id.position_index*3],
					result.attributes.positions[id.position_index*3 + 1],
					result.attributes.positions[id.position_index*3 + 2],
					result.attributes.normals[id.normal_index*3],
					result.attributes.normals[id.normal_index*3 + 1],
					result.attributes.normals[id.normal_index*3 + 2],
					result.attributes.texcoords[id.texcoord_index*2],
					result.attributes.texcoords[id.texcoord_index*2 + 1]
					}
				);
			}
		}

		return std::make_shared<RenderObject>(vertices, indices);
	}

	unsigned int loadTexture(const char* path) {
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			//glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, format, GL_FLOAT, data);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

			//glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);


			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}

	void set_light(Shader* shader, Light* light) {
		shader->setVec3("light.position", light->position);
		shader->setVec3("light.color", light->color);
		shader->setFloat("light.intensity", light->intensity);
	}

	void init_scene() {
		cube = set_cube_ro();
		cat = set_obj_ro("assets/12221_Cat_v1_l3.obj");
		texture_base = loadTexture("assets/wall.jpg");
		texture_cat = loadTexture("assets/Cat_diffuse.jpg");
		shader_phong = std::make_shared<Shader>("assets/vs_phong.glsl", "assets/fs_phong.glsl");
		shader_phong->use();
		material_base = std::make_shared<PhongMaterial>( 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,32, 0, 0, 0 );
		material_d = std::make_shared<PhongMaterial>( 0.2,0.2,0.2,0.8989,1.0,1.0,0.2,0.1,0.1,1, 0, 0, 0 );
		material_s = std::make_shared<PhongMaterial>(0.2, 0.2, 0.2, 0.2, 0.4, 0.23, 0.8849, 1.0, 0.7796, 64, 0., 0., 0);
		material_e = std::make_shared<PhongMaterial>(0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.1, 0.1, 0.1, 0, 1., 1., 0);
		light_main = std::make_shared<Light>(0.0, 2.0, 2.0, 1.0, 1.0, 1.0, 1.0);

		set_light(shader_phong.get(), light_main.get());

		scene.emplace_back(cube, shader_phong);
		{
			auto& model = scene[scene.size() - 1];
			model.texture = texture_base;
			model.material = material_s;
			model.world_transform = make_scaling(15, 0.1, 15);
		}
		scene.emplace_back(cube, shader_phong);
		{
			auto& model = scene[scene.size() - 1];
			model.texture = texture_base;
			model.material = material_e;
			model.world_transform = make_translation(Vec3f{ 0.f, 3.f, 0.f }) *
				make_scaling(0.3, 0.3, 0.3);
		}

		scene.emplace_back(cat, shader_phong);
		{
			auto& model = scene[scene.size() - 1];
			model.texture = texture_cat;
			model.material = material_d;
			model.world_transform = make_translation(Vec3f{ 0.f, .3f, -2.f }) *
				make_rotation_x(-90)*
				make_scaling(0.05, 0.05, 0.05);
		}
		scene.emplace_back(cat, shader_phong);
		{
			auto& model = scene[scene.size() - 1];
			model.texture = texture_cat;
			model.material = material_d;
			model.world_transform = make_translation(Vec3f{ 4.f, .3f, -2.f }) *
				make_rotation_x(-90)*
				make_scaling(0.05, 0.05, 0.05);
		}
		scene.emplace_back(cat, shader_phong);
		{
			auto& model = scene[scene.size() - 1];
			model.texture = texture_cat;
			model.material = material_d;
			model.world_transform = make_translation(Vec3f{ -4.f, .3f, -2.f }) *
				make_rotation_x(-90)*
				make_scaling(0.05, 0.05, 0.05);
		}

	}
	

	void update_scene(Camera& camera) {
		matrix_view = camera.GetViewMatrix();
		matrix_projection = make_perspective_projection(
			PI/2.0,
			(float)WindowControl::_window_width_ / WindowControl::_window_height_,
			0.01f,
			500.f
		);
		
		
		shader_phong->setMat4("view", matrix_view);
		shader_phong->setMat4("projection", matrix_projection);
		shader_phong->setVec3("viewPos", camera.Position);
		
		// time
		// 

		for (int i = 2; i < scene.size(); ++i) {
			Mat44f rot = make_rotation_z(sin(WindowControl::lastFrameTime));
			scene[i].world_transform = scene[i].world_transform * rot;

		}
	}

	void draw_scene() {
		for (auto &model : scene) {
			model.Draw();
		}
	}

}


using namespace WindowControl;
using namespace SceneControl;

int main() try
{
	// Initialize GLFW
	if( GLFW_TRUE != glfwInit() )
	{
		char const* msg = nullptr;
		int ecode = glfwGetError( &msg );
		throw Error( "glfwInit() failed with '%s' (%d)", msg, ecode );
	}

	// Ensure that we call glfwTerminate() at the end of the program.
	GLFWCleanupHelper cleanupHelper;

	// Configure GLFW and create window
	glfwSetErrorCallback( &glfw_callback_error_ );

	glfwWindowHint( GLFW_SRGB_CAPABLE, GLFW_TRUE );
	glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );

	//glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	glfwWindowHint( GLFW_DEPTH_BITS, 24 );

#	if !defined(NDEBUG)
	// When building in debug mode, request an OpenGL debug context. This
	// enables additional debugging features. However, this can carry extra
	// overheads. We therefore do not do this for release builds.
	glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE );
#	endif // ~ !NDEBUG

	GLFWwindow* window = glfwCreateWindow(
		1280,
		720,
		kWindowTitle,
		nullptr, nullptr
	);

	if( !window )
	{
		char const* msg = nullptr;
		int ecode = glfwGetError( &msg );
		throw Error( "glfwCreateWindow() failed with '%s' (%d)", msg, ecode );
	}

	GLFWWindowDeleter windowDeleter{ window };

	// Set up event handling
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback( window, &glfw_callback_key_ );
    glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_);
    glfwSetCursorPosCallback(window, glfw_callback_mouse_);


	// Set up drawing stuff
	glfwMakeContextCurrent( window );
	glfwSwapInterval( 1 ); // V-Sync is on.

	// Initialize GLAD
	// This will load the OpenGL API. We mustn't make any OpenGL calls before this!
	if( !gladLoadGLLoader( (GLADloadproc)&glfwGetProcAddress ) )
		throw Error( "gladLoaDGLLoader() failed - cannot load GL API!" );

	std::printf( "RENDERER %s\n", glGetString( GL_RENDERER ) );
	std::printf( "VENDOR %s\n", glGetString( GL_VENDOR ) );
	std::printf( "VERSION %s\n", glGetString( GL_VERSION ) );
	std::printf( "SHADING_LANGUAGE_VERSION %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );

	// Ddebug output
#	if !defined(NDEBUG)
	setup_gl_debug_output();
#	endif // ~ !NDEBUG

	// Global GL state
	OGL_CHECKPOINT_ALWAYS();

	// TODO: global GL setup goes here

	OGL_CHECKPOINT_ALWAYS();

	// Get actual framebuffer size.
	// This can be different from the window size, as standard window
	// decorations (title bar, borders, ...) may be included in the window size
	// but not be part of the drawable surface area.
	int iwidth, iheight;
	glfwGetFramebufferSize( window, &iwidth, &iheight );

	glViewport( 0, 0, iwidth, iheight );

	// Other initialization & loading
	OGL_CHECKPOINT_ALWAYS();
	
	// TODO: 
	// initialize scene
	init_scene();

	OGL_CHECKPOINT_ALWAYS();

	// Main loop
	while( !glfwWindowShouldClose( window ) )
	{

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrameTime;
        lastFrameTime = currentFrame;
		// Let GLFW process events
		glfwPollEvents();
		
		// Check if window was resized.
		float fbwidth, fbheight;
		{
			int nwidth, nheight;
			glfwGetFramebufferSize( window, &nwidth, &nheight );
			_window_height_ = nheight;
			_window_width_ = nwidth;

			fbwidth = float(nwidth);
			fbheight = float(nheight);

			if( 0 == nwidth || 0 == nheight )
			{
				// Window minimized? Pause until it is unminimized.
				// This is a bit of a hack.
				do
				{
					glfwWaitEvents();
					glfwGetFramebufferSize( window, &nwidth, &nheight );
				} while( 0 == nwidth || 0 == nheight );
			}

			glViewport( 0, 0, nwidth, nheight );
		}

		// Update state

		//TODO: update state
		update_scene(camera);
	
		// Draw scene
		(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		glEnable(GL_DEPTH_TEST);
		OGL_CHECKPOINT_DEBUG();
		draw_scene();

		//TODO: draw frame

		OGL_CHECKPOINT_DEBUG();

		// Display results
		glfwSwapBuffers( window );
	}

	// Cleanup.
	//TODO: additional cleanup
	
	return 0;
}
catch( std::exception const& eErr )
{
	std::fprintf( stderr, "Top-level Exception (%s):\n", typeid(eErr).name() );
	std::fprintf( stderr, "%s\n", eErr.what() );
	std::fprintf( stderr, "Bye.\n" );
	return 1;
}


