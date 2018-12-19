#pragma once
#include "object.h"
#include "DotNetUtilities.h"
#include "Mesh/GUA_OM.h"
#include "Mesh/DP.h"


using namespace glm;
using namespace std;

Tri_Mesh *mesh;
xform xf;
GLCamera camera;
float fov = 0.7f;
//For shader
object model_tri,model_wire,model_point;
size_t window_width = 0;
size_t window_height = 0;
glm::mat4 ProjectionMatrix;
glm::mat4 ViewMatrix;
bool reTime = false;
float countTime = 0;
bool isRotate = false;
bool isTranslate = false;
int xOrigin = -1;
int yOrigin = -1;
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
double cameraDistance = 40;
double translateDelta[3];
double targetPosition[3] = { 0,0,0 };
double ZoomValue = 0.08;

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
	private: System::Windows::Forms::Timer^  timer1;
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
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->loadModelToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->saveModelToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->openModelDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->saveModelDialog = (gcnew System::Windows::Forms::SaveFileDialog());
			this->hkoglPanelControl1 = (gcnew HKOGLPanel::HKOGLPanelControl());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->menuStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->fileToolStripMenuItem });
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(613, 24);
			this->menuStrip1->TabIndex = 1;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
				this->loadModelToolStripMenuItem,
					this->saveModelToolStripMenuItem
			});
			this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			this->fileToolStripMenuItem->Size = System::Drawing::Size(38, 20);
			this->fileToolStripMenuItem->Text = L"File";
			// 
			// loadModelToolStripMenuItem
			// 
			this->loadModelToolStripMenuItem->Name = L"loadModelToolStripMenuItem";
			this->loadModelToolStripMenuItem->Size = System::Drawing::Size(144, 22);
			this->loadModelToolStripMenuItem->Text = L"Load Model";
			this->loadModelToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::loadModelToolStripMenuItem_Click);
			// 
			// saveModelToolStripMenuItem
			// 
			this->saveModelToolStripMenuItem->Name = L"saveModelToolStripMenuItem";
			this->saveModelToolStripMenuItem->Size = System::Drawing::Size(144, 22);
			this->saveModelToolStripMenuItem->Text = L"Save Model";
			this->saveModelToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::saveModelToolStripMenuItem_Click);
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
			hkcoglPanelCameraSetting1->Far = 1000;
			hkcoglPanelCameraSetting1->Fov = 45;
			hkcoglPanelCameraSetting1->Near = -1000;
			hkcoglPanelCameraSetting1->Type = HKOGLPanel::HKCOGLPanelCameraSetting::CAMERATYPE::ORTHOGRAPHIC;
			this->hkoglPanelControl1->Camera_Setting = hkcoglPanelCameraSetting1;
			this->hkoglPanelControl1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->hkoglPanelControl1->Location = System::Drawing::Point(0, 24);
			this->hkoglPanelControl1->Name = L"hkoglPanelControl1";
			hkcoglPanelPixelFormat1->Accumu_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
			hkcoglPanelPixelFormat1->Alpha_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
			hkcoglPanelPixelFormat1->Stencil_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
			this->hkoglPanelControl1->Pixel_Format = hkcoglPanelPixelFormat1;
			this->hkoglPanelControl1->Size = System::Drawing::Size(613, 430);
			this->hkoglPanelControl1->TabIndex = 2;
			this->hkoglPanelControl1->Load += gcnew System::EventHandler(this, &MyForm::hkoglPanelControl1_Load);
			this->hkoglPanelControl1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MyForm::hkoglPanelControl1_Paint);
			this->hkoglPanelControl1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl1_MouseDown);
			this->hkoglPanelControl1->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl1_MouseMove);
			this->hkoglPanelControl1->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl1_MouseUp);
			this->hkoglPanelControl1->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl1_MouseWheel);
			// 
			// timer1
			// 
			this->timer1->Enabled = true;
			this->timer1->Interval = 10;
			this->timer1->Tick += gcnew System::EventHandler(this, &MyForm::timer1_Tick);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(613, 454);
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
private: System::Void hkoglPanelControl1_Load(System::Object^  sender, System::EventArgs^  e)
{
	//glew初始
	GLenum res = glewInit();
	if (res != GLEW_OK) 
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
	}
	glClearColor(0, 0, 0, 1);

	////Set up MVP matrix
	ProjectionMatrix = glm::perspective(60.0f, 613.f / 430.f, 0.1f, 2000.0f);

	ViewMatrix = glm::lookAt(
		glm::vec3(newCameraPosition[0], newCameraPosition[1], newCameraPosition[2]), // Camera in World Space
		glm::vec3(targetPosition[0], targetPosition[1], targetPosition[2]), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,1,0 to look upside-down)
	);
	//Ininialize object
	model_tri.initialize();
	model_tri.setShader("vs.glsl", "fs.glsl");
	model_wire.initialize();
	model_wire.setShader("vs.glsl", "fs.glsl");
	model_point.initialize();
	model_point.setShader("vs.glsl", "fs.glsl");
}
void Update()
{
	glm::mat4 mm(1.0);
	mm *= translate(mat4(1.0), glm::vec3(0, 0, 0));
	mm *= scale(mat4(1.0), glm::vec3(10, 10, 10));
	model_tri.render(GL_TRIANGLES, ProjectionMatrix, mm, ViewMatrix);
	model_wire.render(GL_LINES, ProjectionMatrix, mm, ViewMatrix);
}
private: System::Void hkoglPanelControl1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e)
{
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	UpdateViewMatrix();
	if(mesh != NULL)
		Update();
}
//private: System::Void hkoglPanelControl1_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
//{
//	if (e->Button == System::Windows::Forms::MouseButtons::Left ||
//		e->Button == System::Windows::Forms::MouseButtons::Middle)
//	{
//		point center;
//		Mouse_State = Mouse::NONE;
//		center[0] = 0.0;
//		center[1] = 0.0;
//		center[2] = 0.0;
//		camera.mouse(e->X, e->Y, Mouse_State,
//			xf * center,
//			1.0, xf);
//	}
//}
//private: System::Void hkoglPanelControl1_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
//{
//	if (e->Button == System::Windows::Forms::MouseButtons::Left)
//	{
//		point center;
//		Mouse_State = Mouse::ROTATE;
//		center[0] = 0.0;
//		center[1] = 0.0;
//		center[2] = 0.0;
//		camera.mouse(e->X, e->Y, Mouse_State,
//			xf * center,
//			1.0, xf);
//		hkoglPanelControl1->Invalidate();
//	}
//
//	if (e->Button == System::Windows::Forms::MouseButtons::Middle)
//	{
//		point center;
//		Mouse_State = Mouse::MOVEXY;
//		center[0] = 0.0;
//		center[1] = 0.0;
//		center[2] = 0.0;
//		camera.mouse(e->X, e->Y, Mouse_State,
//			xf * center,
//			1.0, xf);
//		hkoglPanelControl1->Invalidate();
//	}
//}
//private: System::Void hkoglPanelControl1_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
//{
//	if (e->Delta < 0)
//	{
//		point center;
//		Mouse_State = Mouse::WHEELDOWN;
//		center[0] = 0.0;
//		center[1] = 0.0;
//		center[2] = 0.0;
//		camera.mouse(e->X, e->Y, Mouse_State,
//			xf * center,
//			1.0, xf);
//		hkoglPanelControl1->Invalidate();
//	}
//	else
//	{
//		point center;
//		Mouse_State = Mouse::WHEELUP;
//		center[0] = 0.0;
//		center[1] = 0.0;
//		center[2] = 0.0;
//		camera.mouse(e->X, e->Y, Mouse_State,
//			xf * center,
//			1.0, xf);
//		hkoglPanelControl1->Invalidate();
//	}
//}
		private: System::Void hkoglPanelControl1_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
		{
			if (e->Button == System::Windows::Forms::MouseButtons::Left)
			{
				isRotate = true;
				xOrigin = e->X;
				//printf("X:%d\n", xOrigin);
				yOrigin = e->Y;
				//printf("Y:%d\n", yOrigin);
			}
			if (e->Button == System::Windows::Forms::MouseButtons::Middle)
			{
				isTranslate = true;
				xOrigin = e->X;
				yOrigin = e->Y;
			}
			float x = (float)xOrigin / this->Width;			
			float y = (float)yOrigin / this->Height;
			if (mesh != NULL)
			{
				glm::mat4 mm(1.0);
				mm *= translate(mat4(1.0), glm::vec3(0, 0, 0));
				mm *= scale(mat4(1.0), glm::vec3(100, 100, 100));
				glm::vec3 nearest = mesh->nearest_point(glm::vec3(newCameraPosition[0], newCameraPosition[1], newCameraPosition[2]), ProjectionMatrix);
				model_point.setPoint(nearest);
				model_point.setColor(glm::vec3(1,1,1));
				glEnable(GL_PROGRAM_POINT_SIZE);
				glPointSize(8.0);
				model_point.render(GL_POINTS, ProjectionMatrix,mm, ViewMatrix);
				model_point.clear();
				printf("(X,Y,Z):(%f,%f,%f)\n", nearest.x, nearest.y, nearest.z);
			}
			//printf("x/y:%f/%f\n", x,y);
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
		private: System::Void hkoglPanelControl1_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
		{
			if (isRotate)
			{
				angleV -= (e->Y - yOrigin) / 10.0f;
				angleH += (e->X - xOrigin) / 10.0f;
				angleV = (((angleV) > (minAngleV)) ? (angleV) : (minAngleV));
				angleV = (((angleV) < (maxAngleV)) ? (angleV) : (maxAngleV));
				xOrigin = e->X;
				yOrigin = e->Y;
			}
			if (isTranslate)
			{
				double phi = angleH * (3.14159f / 180.0f);
				translateDelta[0] = (e->X - xOrigin) / 10.0f;
				translateDelta[1] = (e->Y - yOrigin) / 10.0f;
				translateDelta[2] = 0.0f;

				translateDelta[2] = translateDelta[0] * cos(phi);
				translateDelta[0] = -1.0*translateDelta[0] * sin(phi);
				xOrigin = e->X;
				yOrigin = e->Y;
			}
		}
		private: System::Void hkoglPanelControl1_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
		{
			if (e->Delta > 0)
			{
				cameraDistance -= 0.05f * abs(cameraDistance);
				cameraDistance = (((cameraDistance) > (minDistance)) ? (cameraDistance) : (minDistance));
			}
			else
			{
				cameraDistance += 0.05f * abs(cameraDistance);
				cameraDistance = (((cameraDistance) > (minDistance)) ? (cameraDistance) : (minDistance));
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
		private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e) 
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
		}
		private: System::Void MyForm_Resize(System::Object^  sender, System::EventArgs^  e) 
		{
			window_width = this->Width;
			window_height = this->Height;			
		}
};
}
