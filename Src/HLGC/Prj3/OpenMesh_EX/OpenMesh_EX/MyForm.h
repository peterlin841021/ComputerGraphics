#pragma once
#include "object.h"
#include "DotNetUtilities.h"
#include "Mesh/GUA_OM.h"
#include "Mesh/DP.h"
//#include "stb_image.h"

using namespace glm;
using namespace std;
typedef unsigned char stbi_uc;
typedef struct _TextureData
{
	_TextureData() : width(0), height(0), data(0) {}
	int width;
	int height;
	unsigned char* data;
}TextureData;
Tri_Mesh *mesh;
xform xf;
xform resetXf;
GLCamera camera;
float fov = 0.7f;
//For shader
object model_tri,model_wire,model_point,model_face,texture_obj;
size_t selectMode = 0;//0:points 1:faces 2:onering
size_t window_width = 0;
size_t window_height = 0;
glm::mat4 ProjectionMatrix;
glm::mat4 ViewMatrix;
glm::mat4 ModelMatrix;
bool reTime = false;
float countTime = 0;
bool isRotate = false;
bool isTranslate = false;
bool multiSelected = false;

int xOrigin = -1;
int yOrigin = -1;
size_t mainPanelXSize = 500;
size_t mainPanelYSize = 500;
double maxAngleV = 180;
double minAngleV = 0;
double minDistance = 0.1;
double angleH = 90;
double angleV = 90;
double prelx = 0.0f;
double prely = 0.0f;
double lx = 0.0f, lz = -1.0f, ly = 0.0f;
std::vector<float> meshes_vs;
std::vector<float> meshes_colors;
double newCameraPosition[3] = { 0,0,-1};
double cameraDistance = 5;
double translateDelta[3];
double targetPosition[3] = { 0,0,0 };
double ZoomValue = 0.05f;

static const Mouse::button physical_to_logical_map[] = 
{
	Mouse::NONE, Mouse::ROTATE, Mouse::MOVEXY, Mouse::MOVEZ,
	Mouse::MOVEZ, Mouse::MOVEXY, Mouse::MOVEXY, Mouse::MOVEXY,
};
Mouse::button Mouse_State = Mouse::ROTATE;

namespace OpenMesh_EX 
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// MyForm 的摘要
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
		public:
			MyForm(void)
			{
				InitializeComponent();
				//
				//TODO:  在此加入建構函式程式碼
				//
			}

		protected:
			/// <summary>
			/// 清除任何使用中的資源。
			/// </summary>
			~MyForm()
			{
				if (components)
				{
					delete components;
				}
			}

		private: System::Windows::Forms::MenuStrip^  menuStrip1;
		private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
		private: System::Windows::Forms::ToolStripMenuItem^  loadModelToolStripMenuItem;
		private: System::Windows::Forms::OpenFileDialog^  openModelDialog;
		private: System::Windows::Forms::SaveFileDialog^  saveModelDialog;
		private: System::Windows::Forms::ToolStripMenuItem^  saveModelToolStripMenuItem;
		private: HKOGLPanel::HKOGLPanelControl^  hkoglPanelControl1;
	private: System::Windows::Forms::Timer^  timer;

	private: System::ComponentModel::BackgroundWorker^  backgroundWorker1;
	private: HKOGLPanel::HKOGLPanelControl^  uv;

	private: System::Windows::Forms::CheckBox^  SelectPoint;
	private: System::Windows::Forms::CheckBox^  SelectFaces;
	private: System::Windows::Forms::CheckBox^  SelectOneRing;
	private: System::Windows::Forms::CheckBox^  MultiSelect;
	private: System::Windows::Forms::ToolStripMenuItem^  loadTextureToolStripMenuItem;
	private: System::Windows::Forms::DateTimePicker^  dateTimePicker2;
	private: HKOGLPanel::HKOGLPanelControl^  texture;
	private: System::Windows::Forms::OpenFileDialog^  openImageFileDialog;


	private: System::ComponentModel::IContainer^  components;
		protected:

		private:
			/// <summary>
			/// 設計工具所需的變數。
			/// </summary>


		#pragma region Windows Form Designer generated code
		/// <summary>
		/// 此為設計工具支援所需的方法 - 請勿使用程式碼編輯器修改
		/// 這個方法的內容。
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			HKOGLPanel::HKCOGLPanelCameraSetting^  hkcoglPanelCameraSetting1 = (gcnew HKOGLPanel::HKCOGLPanelCameraSetting());
			HKOGLPanel::HKCOGLPanelPixelFormat^  hkcoglPanelPixelFormat1 = (gcnew HKOGLPanel::HKCOGLPanelPixelFormat());
			HKOGLPanel::HKCOGLPanelCameraSetting^  hkcoglPanelCameraSetting2 = (gcnew HKOGLPanel::HKCOGLPanelCameraSetting());
			HKOGLPanel::HKCOGLPanelPixelFormat^  hkcoglPanelPixelFormat2 = (gcnew HKOGLPanel::HKCOGLPanelPixelFormat());
			HKOGLPanel::HKCOGLPanelCameraSetting^  hkcoglPanelCameraSetting3 = (gcnew HKOGLPanel::HKCOGLPanelCameraSetting());
			HKOGLPanel::HKCOGLPanelPixelFormat^  hkcoglPanelPixelFormat3 = (gcnew HKOGLPanel::HKCOGLPanelPixelFormat());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->loadModelToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->saveModelToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->loadTextureToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->openModelDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->saveModelDialog = (gcnew System::Windows::Forms::SaveFileDialog());
			this->hkoglPanelControl1 = (gcnew HKOGLPanel::HKOGLPanelControl());
			this->timer = (gcnew System::Windows::Forms::Timer(this->components));
			this->backgroundWorker1 = (gcnew System::ComponentModel::BackgroundWorker());
			this->uv = (gcnew HKOGLPanel::HKOGLPanelControl());
			this->SelectPoint = (gcnew System::Windows::Forms::CheckBox());
			this->SelectFaces = (gcnew System::Windows::Forms::CheckBox());
			this->SelectOneRing = (gcnew System::Windows::Forms::CheckBox());
			this->MultiSelect = (gcnew System::Windows::Forms::CheckBox());
			this->dateTimePicker2 = (gcnew System::Windows::Forms::DateTimePicker());
			this->texture = (gcnew HKOGLPanel::HKOGLPanelControl());
			this->openImageFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->menuStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->fileToolStripMenuItem });
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(784, 24);
			this->menuStrip1->TabIndex = 1;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				this->loadModelToolStripMenuItem,
					this->saveModelToolStripMenuItem, this->loadTextureToolStripMenuItem
			});
			this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			this->fileToolStripMenuItem->Size = System::Drawing::Size(38, 20);
			this->fileToolStripMenuItem->Text = L"File";
			// 
			// loadModelToolStripMenuItem
			// 
			this->loadModelToolStripMenuItem->Name = L"loadModelToolStripMenuItem";
			this->loadModelToolStripMenuItem->Size = System::Drawing::Size(148, 22);
			this->loadModelToolStripMenuItem->Text = L"Load Model";
			this->loadModelToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::loadModelToolStripMenuItem_Click);
			// 
			// saveModelToolStripMenuItem
			// 
			this->saveModelToolStripMenuItem->Name = L"saveModelToolStripMenuItem";
			this->saveModelToolStripMenuItem->Size = System::Drawing::Size(148, 22);
			this->saveModelToolStripMenuItem->Text = L"Save Model";
			this->saveModelToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::saveModelToolStripMenuItem_Click);
			// 
			// loadTextureToolStripMenuItem
			// 
			this->loadTextureToolStripMenuItem->Name = L"loadTextureToolStripMenuItem";
			this->loadTextureToolStripMenuItem->Size = System::Drawing::Size(148, 22);
			this->loadTextureToolStripMenuItem->Text = L"Load Texture";
			this->loadTextureToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::loadTextureToolStripMenuItem_Click);
			// 
			// openModelDialog
			// 
			this->openModelDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &MyForm::openModelDialog_FileOk);
			// 
			// saveModelDialog
			// 
			this->saveModelDialog->DefaultExt = L"obj";
			this->saveModelDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &MyForm::saveModelDialog_FileOk);
			// 
			// hkoglPanelControl1
			// 
			this->hkoglPanelControl1->Anchor = System::Windows::Forms::AnchorStyles::None;
			hkcoglPanelCameraSetting1->Far = 1000;
			hkcoglPanelCameraSetting1->Fov = 45;
			hkcoglPanelCameraSetting1->Near = -1000;
			hkcoglPanelCameraSetting1->Type = HKOGLPanel::HKCOGLPanelCameraSetting::CAMERATYPE::ORTHOGRAPHIC;
			this->hkoglPanelControl1->Camera_Setting = hkcoglPanelCameraSetting1;
			this->hkoglPanelControl1->Location = System::Drawing::Point(12, 27);
			this->hkoglPanelControl1->Name = L"hkoglPanelControl1";
			hkcoglPanelPixelFormat1->Accumu_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
			hkcoglPanelPixelFormat1->Alpha_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
			hkcoglPanelPixelFormat1->Stencil_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
			this->hkoglPanelControl1->Pixel_Format = hkcoglPanelPixelFormat1;
			this->hkoglPanelControl1->Size = System::Drawing::Size(500, 500);
			this->hkoglPanelControl1->TabIndex = 2;
			this->hkoglPanelControl1->Load += gcnew System::EventHandler(this, &MyForm::hkoglPanelControl1_Load);
			this->hkoglPanelControl1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MyForm::hkoglPanelControl1_Paint);
			this->hkoglPanelControl1->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::MyForm_KeyPress);
			this->hkoglPanelControl1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl1_MouseDown);
			this->hkoglPanelControl1->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl1_MouseMove);
			this->hkoglPanelControl1->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl1_MouseUp);
			this->hkoglPanelControl1->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl1_MouseWheel);
			// 
			// timer
			// 
			this->timer->Enabled = true;
			this->timer->Interval = 10;
			this->timer->Tick += gcnew System::EventHandler(this, &MyForm::timer_Tick);
			// 
			// uv
			// 
			this->uv->Anchor = System::Windows::Forms::AnchorStyles::None;
			hkcoglPanelCameraSetting2->Far = 1000;
			hkcoglPanelCameraSetting2->Fov = 45;
			hkcoglPanelCameraSetting2->Near = -1000;
			hkcoglPanelCameraSetting2->Type = HKOGLPanel::HKCOGLPanelCameraSetting::CAMERATYPE::ORTHOGRAPHIC;
			this->uv->Camera_Setting = hkcoglPanelCameraSetting2;
			this->uv->Location = System::Drawing::Point(522, 27);
			this->uv->Name = L"uv";
			hkcoglPanelPixelFormat2->Accumu_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
			hkcoglPanelPixelFormat2->Alpha_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
			hkcoglPanelPixelFormat2->Stencil_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
			this->uv->Pixel_Format = hkcoglPanelPixelFormat2;
			this->uv->Size = System::Drawing::Size(250, 250);
			this->uv->TabIndex = 3;
			this->uv->Load += gcnew System::EventHandler(this, &MyForm::uv_Load);
			this->uv->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MyForm::uv_Paint);
			// 
			// SelectPoint
			// 
			this->SelectPoint->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->SelectPoint->AutoSize = true;
			this->SelectPoint->Location = System::Drawing::Point(12, 534);
			this->SelectPoint->Name = L"SelectPoint";
			this->SelectPoint->Size = System::Drawing::Size(52, 16);
			this->SelectPoint->TabIndex = 5;
			this->SelectPoint->Text = L"Points";
			this->SelectPoint->UseVisualStyleBackColor = true;
			this->SelectPoint->Click += gcnew System::EventHandler(this, &MyForm::SelectPoint_Click);
			// 
			// SelectFaces
			// 
			this->SelectFaces->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->SelectFaces->AutoSize = true;
			this->SelectFaces->Location = System::Drawing::Point(79, 534);
			this->SelectFaces->Name = L"SelectFaces";
			this->SelectFaces->Size = System::Drawing::Size(49, 16);
			this->SelectFaces->TabIndex = 6;
			this->SelectFaces->Text = L"Faces";
			this->SelectFaces->UseVisualStyleBackColor = true;
			this->SelectFaces->Click += gcnew System::EventHandler(this, &MyForm::SelectFaces_Click);
			// 
			// SelectOneRing
			// 
			this->SelectOneRing->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->SelectOneRing->AutoSize = true;
			this->SelectOneRing->Location = System::Drawing::Point(145, 534);
			this->SelectOneRing->Name = L"SelectOneRing";
			this->SelectOneRing->Size = System::Drawing::Size(66, 16);
			this->SelectOneRing->TabIndex = 7;
			this->SelectOneRing->Text = L"OneRing";
			this->SelectOneRing->UseVisualStyleBackColor = true;
			this->SelectOneRing->Click += gcnew System::EventHandler(this, &MyForm::SelectOneRing_Click);
			// 
			// MultiSelect
			// 
			this->MultiSelect->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->MultiSelect->AutoSize = true;
			this->MultiSelect->Location = System::Drawing::Point(229, 533);
			this->MultiSelect->Name = L"MultiSelect";
			this->MultiSelect->Size = System::Drawing::Size(91, 16);
			this->MultiSelect->TabIndex = 8;
			this->MultiSelect->Text = L"MultiSelection";
			this->MultiSelect->UseVisualStyleBackColor = true;
			this->MultiSelect->Click += gcnew System::EventHandler(this, &MyForm::MultiSelect_Click);
			// 
			// dateTimePicker2
			// 
			this->dateTimePicker2->Location = System::Drawing::Point(790, 416);
			this->dateTimePicker2->Name = L"dateTimePicker2";
			this->dateTimePicker2->Size = System::Drawing::Size(200, 22);
			this->dateTimePicker2->TabIndex = 10;
			// 
			// texture
			// 
			this->texture->Anchor = System::Windows::Forms::AnchorStyles::None;
			hkcoglPanelCameraSetting3->Far = 1000;
			hkcoglPanelCameraSetting3->Fov = 45;
			hkcoglPanelCameraSetting3->Near = -1000;
			hkcoglPanelCameraSetting3->Type = HKOGLPanel::HKCOGLPanelCameraSetting::CAMERATYPE::ORTHOGRAPHIC;
			this->texture->Camera_Setting = hkcoglPanelCameraSetting3;
			this->texture->Location = System::Drawing::Point(522, 277);
			this->texture->Name = L"texture";
			hkcoglPanelPixelFormat3->Accumu_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
			hkcoglPanelPixelFormat3->Alpha_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
			hkcoglPanelPixelFormat3->Stencil_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
			this->texture->Pixel_Format = hkcoglPanelPixelFormat3;
			this->texture->Size = System::Drawing::Size(250, 250);
			this->texture->TabIndex = 4;
			this->texture->Load += gcnew System::EventHandler(this, &MyForm::texture_Load);
			this->texture->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MyForm::texture_Paint);
			// 
			// openImageFileDialog
			// 
			this->openImageFileDialog->FileName = L"openFileDialog1";
			this->openImageFileDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &MyForm::openImageFileDialog_FileOk);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(784, 561);
			this->Controls->Add(this->dateTimePicker2);
			this->Controls->Add(this->MultiSelect);
			this->Controls->Add(this->SelectOneRing);
			this->Controls->Add(this->SelectFaces);
			this->Controls->Add(this->SelectPoint);
			this->Controls->Add(this->texture);
			this->Controls->Add(this->uv);
			this->Controls->Add(this->hkoglPanelControl1);
			this->Controls->Add(this->menuStrip1);
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"MyForm";
			this->Text = L"OpenMesh_EX";
			this->Resize += gcnew System::EventHandler(this, &MyForm::MyForm_Resize);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
		#pragma endregion
		void UpdateViewMatrix()
		{
			double theta = angleV * (3.14159f / 180.0f);
			double phi = angleH * (3.14159f / 180.0f);	
			newCameraPosition[0] = cameraDistance * sin(theta) * cos(phi);
			newCameraPosition[1] = cameraDistance * cos(theta);
			newCameraPosition[2] = cameraDistance * sin(theta) * sin(phi);

			for (int i = 0; i < 3; i++)
				targetPosition[i] += translateDelta[i];
			for (int i = 0; i < 3; i++)
				newCameraPosition[i] += targetPosition[i];

			ViewMatrix = glm::lookAt(
				glm::vec3(newCameraPosition[0], newCameraPosition[1], newCameraPosition[2]), // Camera in World Space
				glm::vec3(targetPosition[0], targetPosition[1], targetPosition[2]), // and looks at the origin
				glm::vec3(0, 1, 0)  // Head is up (set to 0,1,0 to look upside-down)
			);
		}
		//Load texture
		void LoadTexture()
		{
			
		}
		private: System::Void hkoglPanelControl1_Load(System::Object^  sender, System::EventArgs^  e)
		{
			//glew初始
			GLenum res = glewInit();
			if (res != GLEW_OK) 
			{
				fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
			}
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			////Set up MVP matrix
			ProjectionMatrix = glm::perspective(60.0f, (float)mainPanelXSize / mainPanelYSize, 0.1f, 1000.0f);
			ModelMatrix = glm::mat4(1.0);			
			ViewMatrix = glm::lookAt(
				glm::vec3(newCameraPosition[0], newCameraPosition[1], newCameraPosition[2]), // Camera in World Space
				glm::vec3(targetPosition[0], targetPosition[1], targetPosition[2]), // and looks at the origin
				glm::vec3(0, 1, 0)  // Head is up (set to 0,1,0 to look upside-down)
			);
			resetXf = xf;
			//Ininialize object
			model_tri.initialize();
			model_tri.setShader("vs.glsl", "fs.glsl");
			model_wire.initialize();
			model_wire.setShader("vs.glsl", "fs.glsl");
			model_point.initialize();
			model_point.setShader("vs.glsl", "fs.glsl");	
			model_face.initialize();
			model_face.setShader("vs.glsl", "fs.glsl");
			texture_obj.initialize();
			texture_obj.setShader("vs.glsl", "fs.glsl");
		}
		private: System::Void uv_Load(System::Object^  sender, System::EventArgs^  e)
		{

		}
		private: System::Void texture_Load(System::Object^  sender, System::EventArgs^  e)
		{	

		}
		void Update()
		{
			glm::vec3 drawColor = glm::vec3(0, 0, 1);
			GLdouble mvm[16];
			glPushMatrix();
				glMatrixMode(GL_MODELVIEW);				
				glMultMatrixd((double *)xf);
				glGetDoublev(GL_MODELVIEW_MATRIX, mvm);
			glPopMatrix();
			mat4 mv = mat4(1.0);
			for (size_t i = 0; i < 4; i++)
			{
				for (size_t j = 0; j < 4; j++)
				{
					mv[i][j] = mvm[i * 4 + j];
				}
			}						
			model_tri.render(GL_TRIANGLES, ProjectionMatrix, mv, ViewMatrix);
			glLineWidth(5);
			model_wire.render(GL_LINES, ProjectionMatrix, mv, ViewMatrix);
			
			glEnable(GL_PROGRAM_POINT_SIZE);
			glPointSize(15);
			model_point.render(GL_POINTS, ProjectionMatrix, mv, ViewMatrix);
			model_face.render(GL_TRIANGLES, ProjectionMatrix, mv, ViewMatrix);
			hkoglPanelControl1->Invalidate();
		}
		private: System::Void hkoglPanelControl1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e)
		{			
			glClearColor(1, 1, 1, 1);						
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LEQUAL);
			glDepthRange(0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			point center;
			center[0] = 0.0;
			center[1] = 0.0;
			center[2] = 0.0;
			camera.setupGL(xf * center, 1.0);
			if (mesh != NULL)
			{
				Update();
			}						
			//UpdateViewMatrix();
		}		
		private: System::Void uv_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) 
		{			
										
		}
		private: System::Void texture_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) 
		{
			glClearColor(0, 0, 0, 1);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LEQUAL);
			glDepthRange(0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		private: System::Void hkoglPanelControl1_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
		{
			if (e->Button == System::Windows::Forms::MouseButtons::Left)
			{
				point center;
				Mouse_State = Mouse::ROTATE;
				center[0] = 0.0;
				center[1] = 0.0;
				center[2] = 0.0;
				camera.mouse(e->X, e->Y, Mouse_State,
					xf * center,
					1.0, xf);
				hkoglPanelControl1->Invalidate();
			}

			if (e->Button == System::Windows::Forms::MouseButtons::Middle)
			{
				point center;
				Mouse_State = Mouse::MOVEXY;
				center[0] = 0.0;
				center[1] = 0.0;
				center[2] = 0.0;
				camera.mouse(e->X, e->Y, Mouse_State,
					xf * center,
					1.0, xf);
				hkoglPanelControl1->Invalidate();
			}
		}
		
		private: System::Void hkoglPanelControl1_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
		{
			if (e->Delta < 0)
			{
				point center;
				Mouse_State = Mouse::WHEELDOWN;
				center[0] = 0.0;
				center[1] = 0.0;
				center[2] = 0.0;
				camera.mouse(e->X, e->Y, Mouse_State,
					xf * center,
					1.0, xf);
				hkoglPanelControl1->Invalidate();
			}
			else
			{
				point center;
				Mouse_State = Mouse::WHEELUP;
				center[0] = 0.0;
				center[1] = 0.0;
				center[2] = 0.0;
				camera.mouse(e->X, e->Y, Mouse_State,
					xf * center,
					1.0, xf);
				hkoglPanelControl1->Invalidate();
			}
		}
		private: System::Void hkoglPanelControl1_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
		{
			UpdateViewMatrix();
			GLdouble pm[16];
			glGetDoublev(GL_PROJECTION_MATRIX, pm);
			GLint viewport[4];
			glGetIntegerv(GL_VIEWPORT, viewport);
			if (e->Button == System::Windows::Forms::MouseButtons::Left || e->Button == System::Windows::Forms::MouseButtons::Middle || e->Button == System::Windows::Forms::MouseButtons::Right)
			{				
				xOrigin = e->X;				
				yOrigin = e->Y;
				if (mesh != NULL)
				{
					point center;
					Mouse_State = Mouse::NONE;
					center[0] = 0.0;
					center[1] = 0.0;
					center[2] = 0.0;
					//Screen to Obj
					glm::vec3 rainbow[7] = { glm::vec3(1, 0, 0),glm::vec3(1, 0.49f,0.15f),glm::vec3(1, 1, 0),glm::vec3(0, 1, 0),glm::vec3(0, 0, 1),glm::vec3(0.24f, 0.28f, 0.8f),glm::vec3(0.63f, 0.28f,0.64f) };
					for (size_t i = 0; i < 4; i++)
					{
						for (size_t j = 0; j < 4; j++)
						{
							pm[i * 4 + j] = ProjectionMatrix[i][j];
						}
					}

					GLfloat  windowZ = 0;
					double objX = 0.f, objY = 0.f, objZ = 0.f;					
					glEnable(GL_DEPTH_TEST);
					glReadPixels(xOrigin, viewport[3] - yOrigin, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &windowZ);
					gluUnProject(xOrigin, viewport[3] - yOrigin, windowZ, xf, pm, viewport, &objX, &objY, &objZ);
					hkoglPanelControl1->Invalidate();
					camera.mouse(xOrigin, yOrigin, Mouse_State, xf * center, 1.0, xf);
					glm::vec3 clickObjCoord = glm::vec3(objX, objY, objZ);
					//
					//printf("OBJ:(%f,%f,%f)\n", objX, objY, objZ);
					/*if (e->Button == System::Windows::Forms::MouseButtons::Left)
					{
						model_point.clear();
						std::vector<glm::vec3> nvs = mesh->nearest_point(clickObjCoord);
						for (size_t i = 0; i < nvs.size(); i++)
						{
							model_point.setPoint(nvs[i]);
						}
						model_point.setColor(drawColor);
					}*/
					if (e->Button == System::Windows::Forms::MouseButtons::Right)
					{
						if (!multiSelected)
						{
							model_point.clear();
							model_face.clear();
						}							
						std::vector<glm::vec3> fs;
						if (selectMode == 0)
						{							
							std::vector<glm::vec3> nvs = mesh->nearest_point(clickObjCoord);
							for (size_t i = 0; i < nvs.size(); i++)
							{
								model_point.setPoint(nvs[i]);								
							}
							for (size_t i = 0; i < model_point.getPointSize(); i++)
							{
								model_point.setColor(rainbow[((i) % 7)]);
							}
						}
						else if (selectMode == 1)
						{
							fs = mesh->nearest_face(clickObjCoord);							
							for (size_t i = 0; i < fs.size(); i++)
							{
								model_face.setPoint(fs[i]);								
							}
							for (size_t i = 0; i < model_face.getPointSize(); i++)
							{
								model_face.setColor(rainbow[(i / 3) % 7]);
							}
						}
						else if (selectMode == 2)
						{
							fs = mesh->nearest_onering(clickObjCoord);
							for (size_t i = 0; i < fs.size(); i++)
							{
								model_face.setPoint(fs[i]);								
							}
							for (size_t i = 0; i < model_face.getPointSize(); i++)
							{
								model_face.setColor(rainbow[(i / 3) % 7]);
							}
						}																		
					}
				}				
			}
		}		
		private: System::Void hkoglPanelControl1_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
		{
			if (e->Button == System::Windows::Forms::MouseButtons::Left)
			{
				isRotate = false;
				prelx = lx;
				prely = ly;
			}
			if (e->Button == System::Windows::Forms::MouseButtons::Middle)
			{
				isTranslate = false;
				translateDelta[0] = 0;
				translateDelta[1] = 0;
				translateDelta[2] = 0;
			}
		}				
		private: System::Void loadModelToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			openModelDialog->Filter = "Model(*.obj)|*obj";
			openModelDialog->Multiselect = false;
			openModelDialog->ShowDialog();
		}
		private: System::Void openModelDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e)
		{
			std::string filename;
			MarshalString(openModelDialog->FileName, filename);

			if (mesh != NULL)
				delete mesh;
			mesh = new Tri_Mesh;			
			if (ReadFile(filename, mesh))
				std::cout << filename << std::endl;
			model_wire.clear();			
			meshes_vs = mesh->GetMesh(0);			
			printf("Size:%d\n", meshes_vs.size());
			for (size_t i = 0; i < meshes_vs.size(); i += 3)
			{
				glm::vec3 p = glm::vec3(meshes_vs[i], meshes_vs[i + 1], meshes_vs[i + 2]);
				model_wire.setPoint(p);
			}
			for (size_t i = 0; i < meshes_vs.size() / 3; i++)
			{
				model_wire.setColor(glm::vec3(0, 0, 0));
			}
			model_tri.clear();
			meshes_vs = mesh->GetMesh(1);
			printf("Size:%d\n", meshes_vs.size());
			for (size_t i = 0; i < meshes_vs.size(); i += 3)
			{
				glm::vec3 p = glm::vec3(meshes_vs[i], meshes_vs[i + 1], meshes_vs[i + 2]);
				model_tri.setPoint(p);
			}
			for (size_t i = 0; i < meshes_vs.size() / 3; i++)
			{
				model_tri.setColor(glm::vec3(1.0, 0.96, 0.49));				
			}
			model_point.clear();
			model_face.clear();
			/**Reset modelview**/
			xf = resetXf;
			point center;
			Mouse_State = Mouse::NONE;
			center[0] = 0.0;
			center[1] = 0.0;
			center[2] = 0.0;
			camera.mouse(0, 0, Mouse_State,
				xf * center,
				1.0, xf);
			hkoglPanelControl1->Invalidate();
		}
		private: System::Void saveModelToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			saveModelDialog->Filter = "Model(*.obj)|*obj";
			saveModelDialog->ShowDialog();		
		}
		private: System::Void saveModelDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e)
		{
			std::string filename;
			MarshalString(saveModelDialog->FileName, filename);

			if (SaveFile(filename, mesh))
				std::cout << filename << std::endl;
		}
		private: System::Void loadTextureToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			openModelDialog->Filter = "Image(*.png)|*png|Image(*.jpg)|*jpg";
			openModelDialog->Multiselect = false;
			openModelDialog->ShowDialog();
		}
		
		stbi_uc *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp)
		{
			FILE *f = fopen(filename, "rb");
			unsigned char *result;
			if (!f) printf("can't fopen", "Unable to open file\n");
			result = stbi_load_from_file(f, x, y, comp, req_comp);
			fclose(f);
			return result;
		}
		TextureData Load_png(const char* path)
		{
			TextureData texture;
			int n;
			stbi_uc *data = stbi_load(path, &texture.width, &texture.height, &n, 4);
			if (data != NULL)
			{
				texture.data = new unsigned char[texture.width * texture.height * 4 * sizeof(unsigned char)];
				memcpy(texture.data, data, texture.width * texture.height * 4 * sizeof(unsigned char));
				// vertical-mirror image data
				for (int i = 0; i < texture.width; i++)
				{
					for (int j = 0; j < texture.height / 2; j++)
					{
						for (size_t k = 0; k < 4; k++) {
							std::swap(texture.data[(j * texture.width + i) * 4 + k], texture.data[((texture.height - j - 1) * texture.width + i) * 4 + k]);
						}
					}
				}
				free(data);
			}
			return texture;
		}
		GLuint generateTexture(const char *image, GLuint id)
		{			
			TextureData *image_data = &(Load_png(image));
			if (!image_data->data)
			{
				printf("Read image fail!\n");
				system("pause");
				return 0;
			}
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, image_data->width, image_data->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data->data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			return id;
		}
		private: System::Void openImageFileDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
		{
			std::string filename;
			MarshalString(openImageFileDialog->FileName, filename);
			
		}
		private: System::Void timer_Tick(System::Object^  sender, System::EventArgs^  e) 
		{
			if (countTime > 80.0f)
				reTime = true;
			if (countTime < -80.0f)
				reTime = false;
			if (reTime)
				countTime -= 1.0f;
			else
				countTime += 1.0f;
			hkoglPanelControl1->Invalidate();
			uv->Invalidate();
			texture->Invalidate();
		}	
		private: System::Void MyForm_Resize(System::Object^  sender, System::EventArgs^  e) 
		{			
		}	
		private: System::Void MyForm_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
		{
			//KeyEvent
		}		
		private: System::Void SelectPoint_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			if (SelectPoint->Checked || !SelectPoint->Checked)
			{
				this->SelectPoint->Checked = true;
				this->SelectFaces->Checked = false;
				this->SelectOneRing->Checked = false;
				selectMode = 0;
			}			
		}
		private: System::Void SelectFaces_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			if (SelectFaces->Checked)
			{
				this->SelectPoint->Checked = false;
				this->SelectFaces->Checked = true;
				this->SelectOneRing->Checked = false;
				selectMode = 1;
			}
			else
			{
				this->SelectPoint->Checked = false;
				this->SelectFaces->Checked = false;
				this->SelectOneRing->Checked = false;
				selectMode = 0;
			}
		}
		private: System::Void SelectOneRing_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			if (SelectOneRing->Checked)
			{
				this->SelectPoint->Checked = false;
				this->SelectFaces->Checked = false;
				this->SelectOneRing->Checked = true;
				selectMode = 2;
			}
			else
			{
				this->SelectPoint->Checked = false;
				this->SelectFaces->Checked = false;
				this->SelectOneRing->Checked = false;
				selectMode = 0;
			}
		}
		private: System::Void MultiSelect_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			if (MultiSelect->Checked)
				multiSelected = true;
			else
				multiSelected = false;
		}	
	};
}
