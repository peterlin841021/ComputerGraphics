#include "GUA_OM.h"

namespace OMT
{
	/*======================================================================*/
	Model::Model()
	{
		request_vertex_status();
		request_edge_status();
		request_face_status();
	}
	Model::~Model()
	{
		release_vertex_status();
		release_edge_status();
		release_face_status();
	}
}
/*======================================================================*/
namespace OMP
{
	Model::Model()
	{
		Mesh.request_vertex_status();
		Mesh.request_edge_status();
		Mesh.request_face_status();
	}
	Model::~Model()
	{
		Mesh.release_vertex_status();
		Mesh.release_edge_status();
		Mesh.release_face_status();
	}
	/*======================================================================*/
	bool Model::ReadFile(std::string _fileName)
	{
		bool isRead = false;
		OpenMesh::IO::Options opt;
		if ( OpenMesh::IO::read_mesh(Mesh, _fileName, opt) )
		{
			//read mesh from filename OK!
			isRead = true;
		}
		if(isRead)
		{
			// If the file did not provide vertex normals and mesh has vertex normal ,then calculate them
			if (!opt.check( OpenMesh::IO::Options::VertexNormal ) && Mesh.has_vertex_normals())
			{
				Mesh.update_normals();
			}
		}
		return isRead;
	}
	bool Model::SaveFile(std::string _fileName)
	{
		bool isSave = false;
		OpenMesh::IO::Options opt;
		if ( OpenMesh::IO::write_mesh(Mesh, _fileName, opt) )
		{
			//read mesh from filename OK!
			isSave = true;
		}
		return isSave;
	}
	/*======================================================================*/
	void Model::Render_solid()
	{
		FIter f_it;
		FVIter	fv_it;
		glPushAttrib(GL_LIGHTING_BIT);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glEnable(GL_DEPTH_TEST);
		glPolygonOffset(2.0, 2.0);
		glBegin(GL_POLYGON);
		//glColor4f(1.0, 0.5, 1.0, 0.5);
		for (f_it = Mesh.faces_begin(); f_it != Mesh.faces_end(); ++f_it) 
		{
			for (fv_it = Mesh.fv_iter( f_it ); fv_it; ++fv_it)
			{						
				glNormal3dv(Mesh.normal(fv_it.handle()).data());
				glVertex3dv(Mesh.point(fv_it.handle()).data());
			}
		}
		glEnd();		
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	void Model::Render_wireframe()
	{
		MyMesh::HalfedgeHandle _hedge;
		EIter e_it=Mesh.edges_begin();

		glDisable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glColor3f(0.0, 0.0, 0.0);
		glLineWidth(1);
		glBegin(GL_LINES);
		for(e_it=Mesh.edges_begin(); e_it != Mesh.edges_end(); ++e_it)
		{
			_hedge = Mesh.halfedge_handle(e_it.handle(),1);

			glVertex3dv(Mesh.point(Mesh.from_vertex_handle(_hedge)).data());
			glVertex3dv(Mesh.point(Mesh.to_vertex_handle(_hedge)).data());			
		}
		glEnd();
		glEnable(GL_LIGHTING);
	}
	/*======================================================================*/
	void Model::RenderSpecifiedPoint()
	{
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glPointSize(5.0f);
		glBegin(GL_POINTS);
		vector<sp_p>::iterator p_itr = sp_p_list.begin();
		for (p_itr; p_itr != sp_p_list.end(); ++p_itr)
		{
			glColor3f(p_itr->r, p_itr->g, p_itr->b);
			glVertex3dv(p_itr->pt.data());
		}
		glEnd();
		glEnable(GL_LIGHTING);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	void Model::RenderSpecifiedVertex()
	{
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glPointSize(5.0f);
		glBegin(GL_POINTS);
		vector< sp_v >::iterator v_itr = sp_v_list.begin();
		for (v_itr; v_itr != sp_v_list.end(); ++v_itr)
		{
			glColor3f(v_itr->r, v_itr->g, v_itr->b);
			glVertex3dv(Mesh.point(v_itr->vh).data());
		}
		glEnd();
		glEnable(GL_LIGHTING);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	void Model::RenderSpecifiedFace()
	{
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glPushAttrib(GL_LIGHTING_BIT);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.5, 1.0);
		glDisable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glBegin(GL_QUADS);
		FVIter fv_itr;
		vector< sp_f >::iterator f_itr;
		for (f_itr = sp_f_list.begin(); f_itr != sp_f_list.end(); ++f_itr)
		{
			glColor3f(f_itr->r, f_itr->g, f_itr->b);
			for (fv_itr=Mesh.fv_iter(f_itr->fh); fv_itr; ++fv_itr)
			{						
				glNormal3dv(Mesh.normal(fv_itr.handle()).data());
				glVertex3dv(Mesh.point(fv_itr.handle()).data());
			}
		}
		glEnd();		
		glEnable(GL_LIGHTING);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT,GL_FILL);
		glEnable(GL_CULL_FACE);
	}

	/*======================================================================*/
	void Model::add_sp_p(Point   _p, float _r, float _g, float _b)
	{
		sp_p input_data;
		input_data.pt = _p;
		input_data.r = _r;
		input_data.g = _g;
		input_data.b = _b;
		sp_p_list.push_back(input_data);
	}
	void Model::add_sp_v(VHandle _v, float _r, float _g, float _b)
	{
		sp_v input_data;
		input_data.vh = _v;
		input_data.r = _r;
		input_data.g = _g;
		input_data.b = _b;
		sp_v_list.push_back(input_data);
	}
	void Model::add_sp_f(FHandle _f, float _r, float _g, float _b)
	{
		sp_f input_data;
		input_data.fh = _f;
		input_data.r = _r;
		input_data.g = _g;
		input_data.b = _b;
		sp_f_list.push_back(input_data);
	}
	void Model::clear_sp_p()
	{
		sp_p_list.clear();
	}
	void Model::clear_sp_v()
	{
		sp_v_list.clear();
	}
	void Model::clear_sp_f()
	{
		sp_f_list.clear();
	}
	/*======================================================================*/
	VHandle Model::addVertex(Point _p)
	{
		int find_result = findVertex(_p);
		if (find_result != -1)
		{
			return Mesh.vertex_handle(find_result);
		}
		else
		{
			return Mesh.add_vertex(_p);
		}
	}
	FHandle Model::addFace(VHandle _v0, VHandle _v1, VHandle _v2, VHandle _v3)
	{
		vector<VHandle> face_vhandles;

		face_vhandles.clear();
		face_vhandles.push_back(_v0);
		face_vhandles.push_back(_v1);
		face_vhandles.push_back(_v2);
		face_vhandles.push_back(_v3);
		return Mesh.add_face(face_vhandles);
	}
	void Model::deleteFace(FHandle _f)
	{
		Mesh.delete_face(_f);
		Mesh.garbage_collection();
	}
	void Model::deleteFace(VHandle _v0, VHandle _v1, VHandle _v2, VHandle _v3)
	{
		/* 
		v1				v0
		*<--------------*
		|				|
		|				|
		|				|
		|		f		|
		|				|
		|				|
		|				|
		* --------------*
		v2				v3
		*/

		HEHandle v0v1 = Mesh.find_halfedge(_v0, _v1);
		if (v0v1.is_valid())
		{
			FHandle fh = Mesh.face_handle(v0v1);
			if (fh.is_valid())
			{
				Mesh.delete_face(fh);
				Mesh.garbage_collection();
			}
		}
	}
	/*======================================================================*/
	bool Model::IsVertexVertex( VHandle _vj, VHandle _vi)
	{
		for( VVIter vvit = Mesh.vv_iter(_vi) ; vvit ; ++vvit )
			if( vvit.handle() == _vj )
				return true;
		return false;
	}
	/*======================================================================*/
	int Model::quad_subdivision1(int _face_id)
	{
		/*----------------------------------------------------------------------*/
		//這段是為了解決index問題
		VHandle vq, vw, vt, vr;
		vq	 = addVertex(Point(0, 0, 100));
		vw	 = addVertex(Point(1, 0, 100));
		vt	 = addVertex(Point(1, 1, 100));
		vr	 = addVertex(Point(0, 1, 100));
		addFace(vq, vw, vt, vr);
		/*----------------------------------------------------------------------*/
		/*收集需要subdivision的face*/
		//初始table
		bool *table = new bool[Mesh.n_faces()];
		for (unsigned i = 0; i < Mesh.n_faces(); i++)
		{
			table[i] = false;
		}

		vector< FHandle > candidate_faces, last_found_face;
		last_found_face.push_back(Mesh.face_handle(_face_id));
		table[_face_id] = true;

		while(last_found_face.size() != 0)
		{
			vector< FHandle > new_found_faces;
			for (vector< FHandle >::iterator crnt_f = last_found_face.begin(); crnt_f != last_found_face.end(); ++crnt_f)
			{
				for (FFIter ff_itr = Mesh.ff_iter(*crnt_f); ff_itr; ++ff_itr)
				{
					if (table[ff_itr.handle().idx()] != true)
					{
						new_found_faces.push_back(ff_itr.handle());
						table[ff_itr.handle().idx()] = true;
					}
				}
			}
			for (vector< FHandle >::iterator f_itr = last_found_face.begin(); f_itr != last_found_face.end(); ++f_itr)
			{
				candidate_faces.push_back(*f_itr);
			}
			last_found_face.assign(new_found_faces.begin(), new_found_faces.end());
		}
		delete table;
		/*----------------------------------------------------------------------*/
		/*對candidate faces做subdivision*/
		/*
			v0		vd		v3
			*-------*-------*
			|		|		|	 
			|		|		|
			|	  ve|		|
		  va*-------*-------*vc
			|		|		|
			|		|		|
			|		|		|
			*-------*-------*
			v1		vb		v2		
		*/
		for (vector< FHandle >::iterator face_itr = candidate_faces.begin(); face_itr != candidate_faces.end(); ++face_itr)
		{
			VHandle v[4], va, vb, vc, vd, ve;
			FVIter fv_itr = Mesh.fv_iter(*face_itr);
			for (int i = 0; fv_itr; ++fv_itr)
			{
				v[i++] = fv_itr.handle();
			}

			deleteFace(v[0], v[1], v[2], v[3]);
			va	 = addVertex((Mesh.point(v[0])+Mesh.point(v[1]))/2);
			vb	 = addVertex((Mesh.point(v[1])+Mesh.point(v[2]))/2);
			vc	 = addVertex((Mesh.point(v[2])+Mesh.point(v[3]))/2);
			vd	 = addVertex((Mesh.point(v[3])+Mesh.point(v[0]))/2);
			ve	 = addVertex((Mesh.point(v[0])+Mesh.point(v[1])+Mesh.point(v[2])+Mesh.point(v[3]))/4);
			addFace(vd, v[0], va, ve);
			addFace(va, v[1], vb, ve);
			addFace(vb, v[2], vc, ve);
			addFace(vc, v[3], vd, ve);
		}
		/*----------------------------------------------------------------------*/
		deleteFace(vq, vw, vt, vr);//這行只是為了解決index問題
		/*----------------------------------------------------------------------*/
		return 0;
	}
	int Model::quad_subdivision2(int _face_id)
	{
		/*----------------------------------------------------------------------*/
		//這段是為了解決index問題
		VHandle vq, vw, vt, vr;
		vq	 = addVertex(Point(0, 0, 100));
		vw	 = addVertex(Point(1, 0, 100));
		vt	 = addVertex(Point(1, 1, 100));
		vr	 = addVertex(Point(0, 1, 100));
		addFace(vq, vw, vt, vr);
		/*----------------------------------------------------------------------*/
		/*收集需要subdivision的face*/
		//初始table
		bool *table = new bool[Mesh.n_faces()];
		for (unsigned i = 0; i < Mesh.n_faces(); i++)
		{
			table[i] = false;
		}

		vector< FHandle > candidate_faces, last_found_face;
		last_found_face.push_back(Mesh.face_handle(_face_id));
		table[_face_id] = true;

		while(last_found_face.size() != 0)
		{
			vector< FHandle > new_found_faces;
			for (vector< FHandle >::iterator crnt_f = last_found_face.begin(); crnt_f != last_found_face.end(); ++crnt_f)
			{
				for (FFIter ff_itr = Mesh.ff_iter(*crnt_f); ff_itr; ++ff_itr)
				{
					if (table[ff_itr.handle().idx()] != true)
					{
						new_found_faces.push_back(ff_itr.handle());
						table[ff_itr.handle().idx()] = true;
					}
				}
			}
			for (vector< FHandle >::iterator f_itr = last_found_face.begin(); f_itr != last_found_face.end(); ++f_itr)
			{
				candidate_faces.push_back(*f_itr);
			}
			last_found_face.assign(new_found_faces.begin(), new_found_faces.end());
		}
		delete table;
		/*----------------------------------------------------------------------*/
		/*對candidate faces做subdivision*/
		/*
			v0		vh		vg		v3
			*-------*-------*-------*
			|		|		|		|
			|		|		|		|
			|	  vi|	  vl|		|
		 va *-------*-------*-------*vf
			|		|		|		|
			|		|		|		|
			|	  vj|	  vk|		|
		 vb *-------*-------*-------*ve
			|		|		|		|
			|		|		|		|
			|		|		|		|
			*-------*-------*-------*
			v1		vc		vd		v2		
		*/
		for (vector< FHandle >::iterator face_itr = candidate_faces.begin(); face_itr != candidate_faces.end(); ++face_itr)
		{
			VHandle v[4], va, vb, vc, vd, ve, vf, vg, vh, vi, vj, vk, vl;
			FVIter fv_itr = Mesh.fv_iter(*face_itr);
			for (int i = 0; fv_itr; ++fv_itr)
			{
				v[i++] = fv_itr.handle();
			}

			deleteFace(v[0], v[1], v[2], v[3]);
			va	 = addVertex((Mesh.point(v[0])*2+Mesh.point(v[1])  )/3);
			vb	 = addVertex((Mesh.point(v[0])  +Mesh.point(v[1])*2)/3);
			vc	 = addVertex((Mesh.point(v[1])*2+Mesh.point(v[2])  )/3);
			vd	 = addVertex((Mesh.point(v[1])  +Mesh.point(v[2])*2)/3);
			ve	 = addVertex((Mesh.point(v[2])*2+Mesh.point(v[3])  )/3);
			vf	 = addVertex((Mesh.point(v[2])  +Mesh.point(v[3])*2)/3);
			vg	 = addVertex((Mesh.point(v[3])*2+Mesh.point(v[0])  )/3);
			vh	 = addVertex((Mesh.point(v[3])  +Mesh.point(v[0])*2)/3);

			vi	 = addVertex((Mesh.point(vh)*2+Mesh.point(vc)  )/3);
			vj	 = addVertex((Mesh.point(vh)  +Mesh.point(vc)*2)/3);
			vk	 = addVertex((Mesh.point(vd)*2+Mesh.point(vg)  )/3);
			vl	 = addVertex((Mesh.point(vd)  +Mesh.point(vg)*2)/3);
			addFace(v[0], va, vi, vh);
			addFace(va, vb, vj, vi);
			addFace(vb, v[1], vc, vj);
			addFace(vc, vd, vk, vj);
			addFace(vd, v[2], ve, vk);
			addFace(ve, vf, vl, vk);
			addFace(vf, v[3], vg, vl);
			addFace(vg, vh, vi, vl);
			addFace(vi, vj, vk, vl);
		}
		/*----------------------------------------------------------------------*/
		deleteFace(vq, vw, vt, vr);//這行只是為了解決index問題
		/*----------------------------------------------------------------------*/
		return 0;
	}
	/*======================================================================*/
	int Model::findVertex(Point _p)
	{
		for( VIter v_itr = Mesh.vertices_begin(); v_itr != Mesh.vertices_end(); ++v_itr)
			if( Mesh.point(v_itr) == _p )
				return v_itr.handle().idx();
		return -1;
	}
	/*======================================================================*/
};
/*======================================================================*/

std::vector<float> Tri_Mesh::GetMesh(size_t type)
{
	FIter f_it;
	FVIter	fv_it;
	std::vector<float> meshes;
	switch (type)
	{
	case 0://LINES
		for (OMT::EIter e_it = edges_begin(); e_it != edges_end(); ++e_it)
		{
			OMT::HEHandle _hedge = halfedge_handle(e_it.handle(), 1);
			OMT::Point curVertex = point(from_vertex_handle(_hedge));
			//glVertex3dv(curVertex.data());
			meshes.push_back(curVertex.data()[0]);
			meshes.push_back(curVertex.data()[1]);
			meshes.push_back(curVertex.data()[2]);
			curVertex = point(to_vertex_handle(_hedge));
			meshes.push_back(curVertex.data()[0]);
			meshes.push_back(curVertex.data()[1]);
			meshes.push_back(curVertex.data()[2]);
			//glVertex3dv(curVertex.data());
		}
		break;
	case 1://TRIANGLES
		for (f_it = faces_begin(); f_it != faces_end(); ++f_it)
		{
			for (fv_it = fv_iter(f_it); fv_it; ++fv_it)
			{
				//glVertex3dv(point(fv_it.handle()).data());
				meshes.push_back(point(fv_it.handle()).data()[0]);
				meshes.push_back(point(fv_it.handle()).data()[1]);
				meshes.push_back(point(fv_it.handle()).data()[2]);
			}
		}
		break;
	case 2://QUADS
		break;
	}
	return meshes;
}
//void Tri_Mesh::Render_Solid()
//{
//	FIter f_it;
//	FVIter	fv_it;
//	//glPushAttrib(GL_LIGHTING_BIT);
//	glEnable(GL_POLYGON_OFFSET_FILL);
//	glEnable(GL_LIGHTING);
//	glPolygonOffset(2.0, 2.0);
//	glBegin(GL_TRIANGLES);
//	glColor4f(0.81, 0.74, 0.33, 0.3);
//	for (f_it = faces_begin(); f_it != faces_end(); ++f_it)
//	{
//		for (fv_it = fv_iter(f_it); fv_it; ++fv_it)
//		{
//			glNormal3dv(normal(fv_it.handle()).data());
//			glVertex3dv(point(fv_it.handle()).data());
//		}
//	}
//	glEnd();
//
//	glDisable(GL_POLYGON_OFFSET_FILL);
//}
//void Tri_Mesh::Render_SolidWireframe()
//{
//	FIter f_it;
//	FVIter	fv_it;
//
//	/*glPointSize(8.0);
//	glBegin(GL_POINTS);
//	glColor4f(1.0, 0.0, 0.0, 1.0);
//	glVertex3dv(point(vertex_handle(0)));
//	glEnd();*/
//    glDisable(GL_LIGHTING);
//	glPushAttrib(GL_LIGHTING_BIT);
//	glEnable(GL_POLYGON_OFFSET_FILL);
//	glEnable(GL_DEPTH_TEST);
//	glPolygonOffset(2.0, 2.0);
//	glBegin(GL_TRIANGLES);
//	glColor4f(1.0, 0.96, 0.49, 1.0);
//	for (f_it = faces_begin(); f_it != faces_end(); ++f_it) 
//	{
//		for (fv_it = fv_iter( f_it ); fv_it; ++fv_it)
//		{						
//			//glNormal3dv(normal(fv_it.handle()));
//			glVertex3dv(point(fv_it.handle()).data());
//		}
//	}
//	glEnd();
//
//	
//	//glDisable(GL_POLYGON_OFFSET_FILL);
//
//
//	glPushAttrib(GL_LIGHTING_BIT);	
//	glDisable(GL_LIGHTING);
//	glLineWidth(1.0);
//	glColor3f(0.0, 0.0, 0.0);	
//	glBegin(GL_LINES);
//	for(OMT::EIter e_it = edges_begin(); e_it != edges_end(); ++e_it)
//	{
//		OMT::HEHandle _hedge = halfedge_handle(e_it.handle(),1);
//
//		OMT::Point curVertex  = point(from_vertex_handle(_hedge));
//		glVertex3dv(curVertex.data());
//
//		curVertex = point(to_vertex_handle(_hedge));
//		glVertex3dv(curVertex.data());			
//	}
//	glEnd();
//	glPopAttrib();
//}

//void Tri_Mesh::Render_Wireframe()
//{
//	//glPushAttrib(GL_LIGHTING_BIT);	
//	glDisable(GL_LIGHTING);
//	glLineWidth(1.0);
//	
//	glColor3f(0.0, 0.0, 0.0);	
//
//	glBegin(GL_LINES);
//	for(OMT::EIter e_it = edges_begin(); e_it != edges_end(); ++e_it)
//	{
//		OMT::HEHandle _hedge = halfedge_handle(e_it.handle(),1);
//
//		OMT::Point curVertex  = point(from_vertex_handle(_hedge));
//		glVertex3dv(curVertex.data());
//
//		curVertex = point(to_vertex_handle(_hedge));
//		glVertex3dv(curVertex.data());			
//	}
//	glEnd();
//	
//}
double Tri_Mesh::seaDragon(Point trangleVertex1, Point trangleVertex2, Point trangleVertex3) {

	double triSide[3];
	double s, triArea;

	triSide[0] = (trangleVertex1 - trangleVertex2).length();
	triSide[1] = (trangleVertex2 - trangleVertex3).length();
	triSide[2] = (trangleVertex3 - trangleVertex1).length();

	s = (triSide[0] + triSide[1] + triSide[2]) / 2;
	triArea = sqrt(s*(s - triSide[0])*(s - triSide[1])*(s - triSide[2]));

	return triArea;	
}
std::vector<glm::vec3> Tri_Mesh::nearest_onering(glm::vec3 screen)
{
	float min_dis = 9999;
	std::vector<glm::vec3> points;
	VHandle vh;
	double triArea[4];
	double mostSuitableTriangle;
	double minimumValue = 0.0001;

	for (OMT::VIter v_it = vertices_begin(); v_it != vertices_end(); ++v_it)
	{
		glm::vec3 p = glm::vec3(point(v_it).data()[0], point(v_it).data()[1], point(v_it).data()[2]);
		float dis = sqrtf(pow(p.x - screen.x, 2) + pow(p.y - screen.y, 2) + pow(p.z - screen.z, 2));
		if (dis < min_dis)
		{
			min_dis = dis;
			vh = v_it.handle();
		}
	}
	if (min_dis <= 9999)
	{
		std::vector<OMT::Point> ring_points;		
		for (OMT::VFIter vf_it = vf_iter(vh); vf_it ; vf_it++)
		{			
			for (OMT::FVIter fv_it = fv_iter(vf_it.handle()); fv_it; fv_it++)
			{
				ring_points.push_back(point(fv_it.handle()));
			}			
		}
		for (size_t i = 0; i < ring_points.size(); i++)
		{
			points.push_back(glm::vec3(ring_points[i][0], ring_points[i][1], ring_points[i][2]));
		}
	}
	return points;
}
std::vector<glm::vec3> Tri_Mesh::nearest_face(glm::vec3 screen)
{
	float min_dis = 9999;
	std::vector<glm::vec3> points;
	VHandle vh;
	double triArea[4];
	double mostSuitableTriangle;
	double minimumValue = 0.0001;
	
	for (OMT::VIter v_it = vertices_begin() ; v_it != vertices_end() ; ++v_it)
	{	
		glm::vec3 p = glm::vec3(point(v_it).data()[0], point(v_it).data()[1], point(v_it).data()[2]);		
		float dis = sqrtf(pow(p.x- screen.x,2)+ pow(p.y- screen.y, 2)+ pow(p.z- screen.z, 2));		
		if (dis < min_dis)
		{
			min_dis = dis;		
			vh = v_it.handle();
		}
	}
	if (min_dis < 9999)
	{
		std::vector<OMT::Point> fp;	
		float minDis = 9999;
		for (OMT::VFIter vf_it = vf_iter(vh); vf_it; vf_it++)
		{
			std::vector<OMT::Point> fv_it_point;
			for (OMT::FVIter fv_it = fv_iter(vf_it.handle()); fv_it; fv_it++)
			{
				fv_it_point.push_back(point(fv_it.handle()));
			}		
			OMT::Point ps = OMT::Point(screen.x, screen.y, screen.z);			
			/*float dis = 0;
			for (size_t i = 0; i < fv_it_point.size(); i++)
			{				
				dis += sqrt(pow(screen.x - fv_it_point[i][0], 2) + pow(screen.y - fv_it_point[i][1], 2) + pow(screen.z - fv_it_point[i][2], 2));				
			}
			if (dis < minDis)
			{
				fp = fv_it_point;
				minDis = dis;
			}*/
			triArea[0] = seaDragon(fv_it_point.at(0), fv_it_point.at(1), fv_it_point.at(2));
			triArea[1] = seaDragon(fv_it_point.at(0),ps, fv_it_point.at(1));
			triArea[2] = seaDragon(fv_it_point.at(1),ps, fv_it_point.at(2));
			triArea[3] = seaDragon(fv_it_point.at(0),ps, fv_it_point.at(2));
			mostSuitableTriangle = (triArea[1] + triArea[2] + triArea[3]);
			if ((-minimumValue < (triArea[0] - mostSuitableTriangle)) && ((triArea[0] - mostSuitableTriangle) < minimumValue))
			{
				fp = fv_it_point;
			}
		}		
		for (size_t i = 0; i < fp.size(); i++)
		{
			points.push_back(glm::vec3(fp[i][0], fp[i][1], fp[i][2]));
		}		
	}	
	return points;
}
std::vector<glm::vec3> Tri_Mesh::nearest_point(glm::vec3 screen)
{
	float min_dis = 9999;
	std::vector<glm::vec3> points;
	VHandle vh;	
	glm::vec3 n;
	for (OMT::VIter v_it = vertices_begin(); v_it != vertices_end(); ++v_it)
	{
		glm::vec3 p = glm::vec3(point(v_it).data()[0], point(v_it).data()[1], point(v_it).data()[2]);
		float dis = sqrtf(pow(p.x - screen.x, 2) + pow(p.y - screen.y, 2) + pow(p.z - screen.z, 2));
		if (dis < min_dis)
		{
			min_dis = dis;			
			n = p;
			vh = v_it.handle();
		}
	}
	if (min_dis < 9999)
	{		
		points.push_back(n);
	}
	return points;
}
void Tri_Mesh::Render_Point()
{	
}

bool ReadFile(std::string _fileName,Tri_Mesh *_mesh)
{
	bool isRead = false;
	OpenMesh::IO::Options opt;
	if ( OpenMesh::IO::read_mesh(*_mesh, _fileName, opt) )
	{
		//read mesh from filename OK!
		isRead = true;
	}
	if(isRead)
	{
		// If the file did not provide vertex normals and mesh has vertex normal ,then calculate them
		if (!opt.check( OpenMesh::IO::Options::VertexNormal ) && _mesh->has_vertex_normals())
		{
			_mesh->update_normals();
		}
	}
	return isRead;
}

bool SaveFile(std::string _fileName, Tri_Mesh *_mesh)
{
	bool isSave = false;
	if (OpenMesh::IO::write_mesh(*_mesh, _fileName))
	{
		isSave = true;
	}
	return isSave;
}
