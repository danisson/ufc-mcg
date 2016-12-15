#include "interface.h"
#include "shapes.h"
#include "octree.h"
#include "csgtree.h"
#include "wed.h"
#include <sstream>
#include <iostream>
#include <random>
#include "helper.h"
#include "raycast.h"
#include <typeindex>
#include <typeinfo>

MainMenu::MainMenu(std::vector<std::unique_ptr<tnw::Model>>& m, IsometricCamera& c) : models(m), camera(c) {
	const size_t m_size = m.size();
	for (size_t i = 0; i < m_size; i++) {
		std::stringstream ss;
		ss << "Modelo " << model_names.size();
		model_names.push_back(ss.str());
	}

	glGenTextures(1, &tex);
}

void MainMenu::errorDialog(const char* name, const char* msg) {
	if (ImGui::BeginPopupModal(name)) {
		ImVec2 buttonSize = ImVec2(0,0);
		float margin = ImGui::GetStyle().ItemSpacing.x;
		buttonSize.x = ImGui::GetWindowContentRegionMax().x - margin;
		ImGui::Text(msg);
		if (ImGui::Button("Okay",buttonSize)) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}

void MainMenu::errorDialog(char* name, char* msg) {
	if (ImGui::BeginPopupModal(name)) {
		ImVec2 buttonSize = ImVec2(0,0);
		float margin = ImGui::GetStyle().ItemSpacing.x;
		buttonSize.x = ImGui::GetWindowContentRegionMax().x - margin;
		ImGui::Text(msg);
		if (ImGui::Button("Okay",buttonSize)) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}

void MainMenu::renderWindow(const GLuint& t, size_t w, size_t h, bool& should_close) {
	ImGui::SetNextWindowSize(ImVec2(w+15,h+35), ImGuiSetCond_FirstUseEver);
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
	if (!ImGui::Begin("Render Window##", &should_close, flags)) {
		ImGui::End();
		return;
	}

	ImGui::Image((void*)(t), ImVec2(w, h), ImVec2(0,0), ImVec2(1,1), ImColor(255,255,255,255), ImColor(255,255,255,128));
	ImGui::End();
}

void MainMenu::draw() {
	ImGui::Begin("Menu");

	if (render_show) renderWindow(tex,raycast_width,raycast_height,render_show);

	if (ImGui::CollapsingHeader("Arquivo")) {
		if (ImGui::Button("Abrir"))
			ImGui::OpenPopup("##abrir_arq");
		ImGui::SameLine();
		if (ImGui::Button("Salvar"))
			ImGui::OpenPopup("##salvar_arq");

		if (ImGui::BeginPopupModal("##abrir_arq")) {
			ImGui::Text("Caminho para o arquivo");
			ImGui::PushItemWidth(-1);
			ImGui::InputText("path",buffer,1000);
			ImGui::PopItemWidth();
			ImVec2 buttonSize = ImVec2(0,0);
			float halfWidth = ImGui::GetWindowContentRegionMax().x*.4f;
			float margin = ImGui::GetStyle().ItemSpacing.x;
			buttonSize.x = halfWidth-margin;
			if (ImGui::Button("Carregar Octree",buttonSize)) {
				FILE* f = nullptr;
				if (strcmp(buffer,"##stdin") == 0) f = stdin;
				else f = fopen(buffer,"r");

				if(!f) {
					ImGui::OpenPopup("Falha Arquivo##1");
				} else {
					size_t size = 0;
					size_t count = model_names.size();
					std::stringstream ss;
					fscanf(f,"%zu\n",&size);
					for (size_t i = 0; i < size; ++i) {
						models.push_back(std::make_unique<tnw::Octree>(f));
						ss << "Árvore " << (count+i) << "[ARQUIVO]";
						model_names.push_back(ss.str());
						ss.str(std::string());ss.clear();
					}
					fclose(f);
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Carregar CSG",buttonSize)) {
				FILE* f = nullptr;
				if (strcmp(buffer,"##stdin") == 0) f = stdin;
				else f = fopen(buffer,"r");

				if(!f) {
					ImGui::OpenPopup("Falha Arquivo##1");
				} else {
					size_t size = 0;
					size_t count = model_names.size();
					std::stringstream ss;
					fscanf(f,"%zu\n",&size);
					for (size_t i = 0; i < size; ++i) {
						models.push_back(std::make_unique<tnw::CSGTree>(f));
						ss << "Árvore " << (count+i) << "[ARQUIVO]";
						model_names.push_back(ss.str());
						ss.str(std::string());ss.clear();
					}
					fclose(f);
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancelar",buttonSize)) {ImGui::CloseCurrentPopup();}

			errorDialog("Falha Arquivo##1","Arquivo não existente ou não pôde ser aberto.");
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("##salvar_arq")) {
			ImGui::Text("Caminho para o arquivo");
			ImGui::PushItemWidth(-1);
			ImGui::InputText("path",buffer,1000);
			ImGui::PopItemWidth();
			ImVec2 buttonSize = ImVec2(0,0);
			float halfWidth = ImGui::GetWindowContentRegionMax().x*.5f;
			float margin = ImGui::GetStyle().ItemSpacing.x;
			buttonSize.x = halfWidth-margin;
			if (ImGui::Button("Salvar",buttonSize)) {
				FILE* f = nullptr;
				if (strcmp(buffer,"##stdout") == 0) f = stdout;
				else f = fopen(buffer,"w");

				if(!f) {
					ImGui::OpenPopup("Falha Arquivo##2");
				} else {
					fprintf(f,"%zu\n", models.size());
					for (auto&& m : models) {
						fprintf(f,"%s\n",m->serialize().c_str());
					}
					fclose(f);
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancelar",buttonSize)) {ImGui::CloseCurrentPopup();}

			errorDialog("Falha Arquivo##2","Arquivo não existente ou não pôde ser escrito.");
			ImGui::EndPopup();
		}
	}

	if (ImGui::CollapsingHeader("Objetos")) {

		const char* tree_names[model_names.size()];

		for (unsigned int i = 0; i < model_names.size(); i++) {
			tree_names[i] = model_names[i].c_str();
		}

		static int curr_item = -1;

		ImGui::PushItemWidth(-1);
		ImGui::ListBox("##cena", &curr_item, tree_names, models.size(), models.size());
		ImGui::PopItemWidth();

		bool isSelected = curr_item >= 0 && static_cast<size_t>(curr_item) < models.size();
		bool isBRep = isSelected && (std::type_index(typeid(*models[curr_item])) == std::type_index(typeid(tnw::BRep)));


		ImVec2 buttonSize = ImVec2(0,0);
		float fullWidth = ImGui::GetWindowContentRegionMax().x;
		float margin = ImGui::GetStyle().ItemSpacing.x;
		buttonSize.x = fullWidth/2-margin;

		if (isBRep) {
			auto* m = (tnw::BRep*)models[curr_item].get();
			buttonSize.x = fullWidth/3-margin;
			if (ImGui::Button("Adjacencia",buttonSize) && !!m->vertices.size()){
				m->clear_marks();
				ImGui::OpenPopup("Adjacencias");
			}
			ImGui::SameLine();
			if (ImGui::BeginPopupModal("Adjacencias", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImVec2 buttonSize = ImVec2(0,0);
				const float margin = ImGui::GetStyle().ItemSpacing.x;
				const float width = ImGui::GetWindowContentRegionMax().x;

				ImGui::Text("id:");
				ImGui::InputInt("id##loop", (int*)&brep_ids[0]);
				ImGui::Separator();

				buttonSize.x = width/2.0-margin;
				if (ImGui::Button("LV", buttonSize)) {
					auto l = m->get_loop(brep_ids[0]);
					if (!l) goto failGet;
					for (auto x : l->adjvertex()) {
						m->mark_vertex(x->id);
					}
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("LE", buttonSize)) {
					auto l = m->get_loop(brep_ids[0]);
					if (!l) goto failGet;
					for (auto x : l->adjedge()) {
						m->mark_edge(x->id);
					}
					ImGui::CloseCurrentPopup();
				}
				buttonSize.x = width/2.0-margin;
				if (ImGui::Button("EV", buttonSize)) {
					m->print_info();
					auto l = m->get_wedge(brep_ids[0]);
					if (!l) goto failGet;
					m->selected_edge = l->id;
					for (auto x : l->adjvertex()) {
						m->mark_vertex(x->id);
					}
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("EE", buttonSize)) {
					auto l = m->get_wedge(brep_ids[0]);
					if (!l) goto failGet;
					m->selected_edge = l->id;
					for (auto x : l->adjedge()) {
						m->mark_edge(x->id);
					}
					ImGui::CloseCurrentPopup();
				}
				buttonSize.x = width/2.0-margin;
				if (ImGui::Button("VV", buttonSize)) {
					m->print_info();
					auto l = m->get_vertex(brep_ids[0]);
					if (!l) goto failGet;
					// m->selected_edge = l->id;
					for (auto x : l->adjvertex()) {
						m->mark_vertex(x->id);
					}
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("VE", buttonSize)) {
					auto l = m->get_vertex(brep_ids[0]);
					if (!l) goto failGet;
					// m->selected_edge = l->id;
					for (auto x : l->adjedge()) {
						m->mark_edge(x->id);
					}
					ImGui::CloseCurrentPopup();
				}
				buttonSize.x = width-margin;
				if (ImGui::Button("Cancel", buttonSize)) {
					m->print_info();
					ImGui::CloseCurrentPopup();
				}
				failGet:
				ImGui::EndPopup();
			}
		}

		if (ImGui::Button("Translação",buttonSize) && isSelected) {
			ImGui::OpenPopup("Parâmetros da Translação");
		}
		ImGui::SameLine();
		if (ImGui::Button("Escala",buttonSize) && isSelected) {
			ImGui::OpenPopup("Parâmetros da Escala");
		}

		if (ImGui::BeginPopupModal("Parâmetros da Translação", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::InputFloat("x", &x);
			ImGui::InputFloat("y", &y);
			ImGui::InputFloat("z", &z);

			if (ImGui::Button("OK", ImVec2(120,0))) {
				if (isSelected) {
					models[curr_item]->translate({x,y,z});
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Parâmetros da Escala")) {
			ImGui::InputFloat("r", &x);

			if (ImGui::Button("OK", ImVec2(120,0))) {
				if (isSelected) {
					models[curr_item]->scale(x);
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}

		// ImGui::SameLine();
		if (isBRep) {
			auto* m = (tnw::BRep*)models[curr_item].get();
			buttonSize.x = fullWidth/3-margin;
			if (ImGui::Button("MVFS",buttonSize) && !m->vertices.size()) {
				ImGui::OpenPopup("MVFS##2");
			}
			if (ImGui::BeginPopupModal("MVFS##2", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImVec2 buttonSize = ImVec2(0,0);
				float halfWidth = ImGui::GetWindowContentRegionMax().x*.5f;
				float margin = ImGui::GetStyle().ItemSpacing.x;
				buttonSize.x = halfWidth-margin;
				ImGui::Text("novo vértice:");
				ImGui::InputFloat("x", &x);
				ImGui::InputFloat("y", &y);
				ImGui::InputFloat("z", &z);
				ImGui::Separator();

				if (ImGui::Button("OK", buttonSize)) {
					m->mvfs(glm::vec3{x,y,z});
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", buttonSize))
					ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("MEV",buttonSize) && !!m->vertices.size()) {
				ImGui::OpenPopup("MEV##2");
			}
			if (ImGui::BeginPopupModal("MEV##2", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImVec2 buttonSize = ImVec2(0,0);
				float halfWidth = ImGui::GetWindowContentRegionMax().x*.5f;
				float margin = ImGui::GetStyle().ItemSpacing.x;
				buttonSize.x = halfWidth-margin;

				ImGui::Text("loop:");
				ImGui::InputInt("id##loop", (int*)&brep_ids[0]);
				ImGui::Separator();
				ImGui::Text("vértice:");
				ImGui::InputInt("id##vertice", (int*)&brep_ids[1]);
				ImGui::Separator();
				ImGui::Text("novo vértice:");
				ImGui::InputFloat("x", &x);
				ImGui::InputFloat("y", &y);
				ImGui::InputFloat("z", &z);
				ImGui::Separator();

				if (ImGui::Button("OK", buttonSize)) {
					m->smev(brep_ids[0],brep_ids[1],glm::vec3{x,y,z});
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", buttonSize))
					ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("MEF",buttonSize) && !!m->vertices.size()) {
				ImGui::OpenPopup("MEF##2");
			}
			if (ImGui::BeginPopupModal("MEF##2", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImVec2 buttonSize = ImVec2(0,0);
				float halfWidth = ImGui::GetWindowContentRegionMax().x*.5f;
				float margin = ImGui::GetStyle().ItemSpacing.x;
				buttonSize.x = halfWidth-margin;

				ImGui::Text("loop:");
				ImGui::InputInt("id##loop", (int*)&brep_ids[0]);
				ImGui::Separator();
				ImGui::Text("vértices:");
				ImGui::InputInt4("id##vertice", &brep_ids[1]);
				ImGui::Separator();

				if (ImGui::Button("OK", buttonSize)) {
					m->mef(brep_ids[0],brep_ids[1],brep_ids[2],brep_ids[3],brep_ids[4]);
					// m->print_info();
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", buttonSize))
					ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}
		} else {
			buttonSize.x = fullWidth/2-margin;
			if (ImGui::Button("União",buttonSize) && isSelected) {
				ImGui::OpenPopup("Parâmetros de União");
			}

			if (ImGui::BeginPopupModal("Parâmetros de União", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				static int selected_or = -1;
				ImGui::Combo("selecione a árvore com que operar", &selected_or, tree_names, model_names.size());


				if (ImGui::Button("OK", ImVec2(120,0)) && (selected_or >= 0 && static_cast<size_t>(selected_or) < models.size())) {
					tnw::BooleanErrorCodes result;

					result = models[curr_item]->bool_or(*models[selected_or]);

					if (result == tnw::BooleanErrorCodes::unimplementedType) { open_type_error_popup = true; }
					else if (result == tnw::BooleanErrorCodes::boundingboxMismatch) { open_bb_mismatch_error_popup = true; }
					else { model_names[curr_item] = model_names[curr_item].append(" OR ").append(model_names[selected_or]); }
					ImGui::CloseCurrentPopup();

				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }

				ImGui::EndPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Interseção",buttonSize) && isSelected) {
				ImGui::OpenPopup("Parâmetros de Interseção");
			}

			if (ImGui::BeginPopupModal("Parâmetros de Interseção", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

				static int selected_and = -1;
				ImGui::Combo("selecione a árvore com que operar", &selected_and, tree_names, model_names.size());

				if (ImGui::Button("OK", ImVec2(120,0)) && (selected_and >= 0 && static_cast<unsigned int>(selected_and) < models.size())) {
					tnw::BooleanErrorCodes result;

					result = models[curr_item]->bool_and(*models[selected_and]);

					if (result == tnw::BooleanErrorCodes::unimplementedType) { open_type_error_popup = true; }
					else if (result == tnw::BooleanErrorCodes::boundingboxMismatch) { open_bb_mismatch_error_popup = true; }
					else { model_names[curr_item] = model_names[curr_item].append(" AND ").append(model_names[selected_and]); }

					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}
		}

		if (open_type_error_popup) {
			// std::cout << "error popup should open!\n";
			ImGui::OpenPopup("Erro de tipo");
		}

		if (ImGui::BeginPopupModal("Erro de tipo")) {
			ImGui::Text("Os tipos dos objetos selecionados não são compatíveis");
			if (ImGui::Button("OK")) { ImGui::CloseCurrentPopup(); open_type_error_popup = false;}
			ImGui::EndPopup();
		}

		if (open_bb_mismatch_error_popup) {
			// std::cout << "bb popup should open!\n";
			ImGui::OpenPopup("Erro de bounding box");
		}

		if (ImGui::BeginPopupModal("Erro de bounding box")) {
			ImGui::Text("Os objetos tem bounding boxes diferentes");
			if (ImGui::Button("OK")) { ImGui::CloseCurrentPopup(); open_bb_mismatch_error_popup = false;}
			ImGui::EndPopup();
		}

		if (!isBRep) {
			// ImGui::SameLine();
			buttonSize.x = fullWidth/1-margin;
			if (ImGui::Button("Volume",buttonSize) && (curr_item >= 0 && static_cast<unsigned int>(curr_item) < models.size())) {
				volumeCache = models[curr_item]->volume();
				ImGui::OpenPopup("Volume##2");
			}
			if (ImGui::BeginPopup("Volume##2")) {
				ImGui::Text("%lf",volumeCache);
				ImGui::EndPopup();
			}
		}


		buttonSize.x = fullWidth/4-margin;
		if (ImGui::Button("Cor",buttonSize) && (curr_item >= 0 && static_cast<unsigned int>(curr_item) < models.size())) {
			ImGui::OpenPopup("Parâmetros de Cor");
		}

		if (ImGui::BeginPopupModal("Parâmetros de Cor", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::ColorEdit3("cor", color);
			if (ImGui::Button("Cor aleatória")) {
				std::random_device r;
				std::default_random_engine gen(r());

				std::uniform_real_distribution<> dis(0,1);

				for (int i = 0; i < 3; ++i)
					color[i] = dis(gen);
			}
			if (ImGui::Button("OK", ImVec2(120,0))) {
				models[curr_item]->setColor(color);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		const char* text;
		if (isSelected && models[curr_item]->visible) text = esconder_text;
		else text = mostrar_text;

		if (ImGui::Button(text,buttonSize) && isSelected) {
			models[curr_item]->toggle();
		}
		ImGui::SameLine();
		if (ImGui::Button("Remover",buttonSize) && isSelected) {
			models.erase(models.begin() + curr_item);
			model_names.erase(model_names.begin() + curr_item);
		}
		ImGui::SameLine();
		if (ImGui::Button("Duplicar",buttonSize) && isSelected) {
			models.push_back(unique_ptr<tnw::Model>((*(models.begin() + curr_item))->clone()));
			model_names.push_back("Clone " + std::to_string(model_names.size()));
		}

		ImGui::PushID("Octree");
		if (ImGui::CollapsingHeader("Nova Octree")) {
			if (ImGui::Button("Esfera")) {
				ImGui::OpenPopup("Parâmetros da Esfera");
			}
			if (ImGui::BeginPopupModal("Parâmetros da Esfera", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::Text("Bounding Box:");
				ImGui::InputFloat("bx", &bx);
				ImGui::InputFloat("by", &by);
				ImGui::InputFloat("bz", &bz);
				ImGui::InputFloat("bd", &bd);
				ImGui::Separator();
				ImGui::Text("Maximum depth:");
				ImGui::InputInt("md", &md,1,2);
				ImGui::Separator();
				ImGui::Text("centro:");
				ImGui::InputFloat("x", &x);
				ImGui::InputFloat("y", &y);
				ImGui::InputFloat("z", &z);
				ImGui::Separator();
				ImGui::InputFloat("raio", &r);

				if (ImGui::Button("OK", ImVec2(120,0))) {
					tnw::BoundingBox b({bx,by,bz},bd);
					tnw::Sphere s({x,y,z}, r);
					models.push_back(std::make_unique<tnw::Octree>(s,b,md));
					std::stringstream ss;
					ss << "Árvore " << model_names.size() << "[ESFERA]";
					model_names.push_back(ss.str());
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Caixa")) {
				ImGui::OpenPopup("Parâmetros da Caixa");
			}
			if (ImGui::BeginPopupModal("Parâmetros da Caixa", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Bounding Box:");
				ImGui::InputFloat("bx", &bx);
				ImGui::InputFloat("by", &by);
				ImGui::InputFloat("bz", &bz);
				ImGui::InputFloat("bd", &bd);
				ImGui::Separator();
				ImGui::Text("Maximum depth:");
				ImGui::InputInt("md", &md,1,2);
				ImGui::Separator();
				ImGui::Text("centro:");
				ImGui::InputFloat("x", &x);
				ImGui::InputFloat("y", &y);
				ImGui::InputFloat("z", &z);
				ImGui::Separator();
				ImGui::InputFloat("largura", &l);
				ImGui::InputFloat("altura", &h);
				ImGui::InputFloat("profundidade", &d);

				if (ImGui::Button("OK", ImVec2(120,0))) {
					tnw::Box bb({x,y,z}, l, h, d);
					tnw::BoundingBox b({bx,by,bz},bd);
					models.push_back(std::make_unique<tnw::Octree>(bb,b,md));
					std::stringstream ss;
					ss << "Árvore " << model_names.size() << "[CAIXA]";
					model_names.push_back(ss.str());
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Cilindro")) {
				ImGui::OpenPopup("Parâmetros do Cilindro");
			}
			if (ImGui::BeginPopupModal("Parâmetros do Cilindro", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Bounding Box:");
				ImGui::InputFloat("bx", &bx);
				ImGui::InputFloat("by", &by);
				ImGui::InputFloat("bz", &bz);
				ImGui::InputFloat("bd", &bd);
				ImGui::Separator();
				ImGui::Text("Maximum depth:");
				ImGui::InputInt("md", &md,1,2);
				ImGui::Separator();
				ImGui::Text("centro do lado inferior:");
				ImGui::InputFloat("x", &x);
				ImGui::InputFloat("y", &y);
				ImGui::InputFloat("z", &z);

				ImGui::Separator();
				ImGui::InputFloat("raio", &r);
				ImGui::InputFloat("altura", &h);

				if (ImGui::Button("OK", ImVec2(120,0))) {
					tnw::Cilinder cl({x,y,z}, h, r);
					tnw::BoundingBox b({bx,by,bz},bd);
					models.push_back(std::make_unique<tnw::Octree>(cl,b,md));
					std::stringstream ss;
					ss << "Árvore " << model_names.size() << "[CILINDRO]";
					model_names.push_back(ss.str());
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Pirâmide Base Quadrada")) {
				ImGui::OpenPopup("Parâmetros da Pirâmide");
			}
			if (ImGui::BeginPopupModal("Parâmetros da Pirâmide", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Bounding Box:");
				ImGui::InputFloat("bx", &bx);
				ImGui::InputFloat("by", &by);
				ImGui::InputFloat("bz", &bz);
				ImGui::InputFloat("bd", &bd);
				ImGui::Separator();
				ImGui::Text("Maximum depth:");
				ImGui::InputInt("md", &md,1,2);
				ImGui::Separator();
				ImGui::Text("centro do lado inferior:");
				ImGui::InputFloat("x", &x);
				ImGui::InputFloat("y", &y);
				ImGui::InputFloat("z", &z);

				ImGui::Separator();
				ImGui::InputFloat("altura", &h);
				ImGui::InputFloat("lado", &l);

				if (ImGui::Button("OK", ImVec2(120,0))) {
					tnw::SquarePyramid sp({x,y,z}, h, l);
					tnw::BoundingBox b({bx,by,bz},bd);
					models.push_back(std::make_unique<tnw::Octree>(sp,b,md));
					std::stringstream ss;
					ss << "Árvore " << model_names.size() << "[PIRÂMIDE]";
					model_names.push_back(ss.str());
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}

			if (ImGui::Button("Modelo")) {
				if (isSelected)
					ImGui::OpenPopup("Parâmetros da Classificação");
				else
					ImGui::OpenPopup("Erro de seleção");
			}
			if (ImGui::BeginPopupModal("Parâmetros da Classificação")) {
				ImGui::Text("Bounding Box:");
				ImGui::InputFloat("bx", &bx);
				ImGui::InputFloat("by", &by);
				ImGui::InputFloat("bz", &bz);
				ImGui::InputFloat("bd", &bd);
				ImGui::Separator();
				ImGui::Text("Maximum depth:");
				ImGui::InputInt("md", &md,1,2);

				if (ImGui::Button("OK", ImVec2(120,0))) {
					tnw::BoundingBox b({bx,by,bz},bd);
					models.push_back(std::make_unique<tnw::Octree>(*models[curr_item],b,md));
					std::stringstream ss;
					ss << "Árvore " << model_names.size() << "[TREE]";
					model_names.push_back(ss.str());
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}
			errorDialog("Erro de seleção","Selecione um objeto.");
		}
		ImGui::PopID();

		ImGui::PushID("CSG");
		if (ImGui::CollapsingHeader("Nova CSG")) {
			if (ImGui::Button("Esfera")) {
				ImGui::OpenPopup("Parâmetros da Esfera");
			}
			if (ImGui::BeginPopupModal("Parâmetros da Esfera")) {
				ImGui::Text("centro:");
				ImGui::InputFloat("x", &x);
				ImGui::InputFloat("y", &y);
				ImGui::InputFloat("z", &z);
				ImGui::Separator();
				ImGui::InputFloat("raio", &r);

				if (ImGui::Button("OK", ImVec2(120,0))) {
					auto s = new tnw::Sphere({x,y,z}, r);
					models.push_back(std::make_unique<tnw::CSGTree>(s));
					std::stringstream ss;
					ss << "CSG " << model_names.size() << "[ESFERA]";
					model_names.push_back(ss.str());
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Caixa")) {
				ImGui::OpenPopup("Parâmetros da Caixa");
			}
			if (ImGui::BeginPopupModal("Parâmetros da Caixa")) {
				ImGui::Text("centro:");
				ImGui::InputFloat("x", &x);
				ImGui::InputFloat("y", &y);
				ImGui::InputFloat("z", &z);
				ImGui::Separator();
				ImGui::InputFloat("largura", &l);
				ImGui::InputFloat("altura", &h);
				ImGui::InputFloat("profundidade", &d);

				if (ImGui::Button("OK", ImVec2(120,0))) {
					auto bb = new tnw::Box({x,y,z}, l, d, h);
					models.push_back(std::make_unique<tnw::CSGTree>(bb));
					std::stringstream ss;
					ss << "CSG " << model_names.size() << "[CAIXA]";
					model_names.push_back(ss.str());
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Cilindro")) {
				ImGui::OpenPopup("Parâmetros do Cilindro");
			}
			if (ImGui::BeginPopupModal("Parâmetros do Cilindro")) {
				ImGui::Text("centro do lado inferior:");
				ImGui::InputFloat("x", &x);
				ImGui::InputFloat("y", &y);
				ImGui::InputFloat("z", &z);

				ImGui::Separator();
				ImGui::InputFloat("raio", &r);
				ImGui::InputFloat("altura", &h);

				if (ImGui::Button("OK", ImVec2(120,0))) {
					auto cl = new tnw::Cilinder({x,y,z}, h, r);
					models.push_back(std::make_unique<tnw::CSGTree>(cl));
					std::stringstream ss;
					ss << "CSG " << model_names.size() << "[CILINDRO]";
					model_names.push_back(ss.str());
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Pirâmide Base Quadrada")) {
				ImGui::OpenPopup("Parâmetros da Pirâmide");
			}
			if (ImGui::BeginPopupModal("Parâmetros da Pirâmide")) {
				ImGui::Text("centro do lado inferior:");
				ImGui::InputFloat("x", &x);
				ImGui::InputFloat("y", &y);
				ImGui::InputFloat("z", &z);

				ImGui::Separator();
				ImGui::InputFloat("altura", &h);
				ImGui::InputFloat("lado", &l);

				if (ImGui::Button("OK", ImVec2(120,0))) {
					auto sp = new tnw::SquarePyramid({x,y,z}, h, l);
					models.push_back(std::make_unique<tnw::CSGTree>(sp));
					std::stringstream ss;
					ss << "CSG " << model_names.size() << "[PIRÂMIDE]";
					model_names.push_back(ss.str());
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}

			if (ImGui::Button("Modelo")) {
				if (isSelected)
					ImGui::OpenPopup("Parâmetros da Classificação");
				else
					ImGui::OpenPopup("Erro de seleção");
			}
			if (ImGui::BeginPopupModal("Parâmetros da Classificação")) {
				if (ImGui::Button("OK", ImVec2(120,0))) {
					auto c = models[curr_item]->clone();
					models.push_back(std::make_unique<tnw::CSGTree>(c));
					std::stringstream ss;
					ss << "CSG " << model_names.size() << "[MODEL]";
					model_names.push_back(ss.str());
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}
			errorDialog("Erro de seleção","Selecione um objeto.");
		}
		ImGui::PopID();

		ImGui::PushID("Brep");
		if (ImGui::CollapsingHeader("Nova Brep")) {
			if (ImGui::Button("Cubo")) {
				models.push_back(std::make_unique<tnw::BRep>(9,13,7));
				std::stringstream ss;
				ss << "BRep " << model_names.size() << "[CUBO]";
				model_names.push_back(ss.str());
				auto mdl = (tnw::BRep*)models.back().get();

				using namespace tnw::wed;

				mdl->edges.emplace_front(1);WEdge *a = &mdl->edges.front();
				mdl->edges.emplace_front(2);WEdge *b = &mdl->edges.front();
				mdl->edges.emplace_front(3);WEdge *c = &mdl->edges.front();
				mdl->edges.emplace_front(4);WEdge *d = &mdl->edges.front();
				mdl->edges.emplace_front(5);WEdge *e = &mdl->edges.front();
				mdl->edges.emplace_front(6);WEdge *f = &mdl->edges.front();
				mdl->edges.emplace_front(7);WEdge *g = &mdl->edges.front();
				mdl->edges.emplace_front(8);WEdge *h = &mdl->edges.front();
				mdl->edges.emplace_front(9);WEdge *i = &mdl->edges.front();
				mdl->edges.emplace_front(10);WEdge *j = &mdl->edges.front();
				mdl->edges.emplace_front(11);WEdge *k = &mdl->edges.front();
				mdl->edges.emplace_front(12);WEdge *l = &mdl->edges.front();

				mdl->vertices.emplace_front(1, glm::vec3{0,0,0}, a);
				Vertex *A = &mdl->vertices.front();
				mdl->vertices.emplace_front(2, glm::vec3{0,1,0},b);
				Vertex *B = &mdl->vertices.front();
				mdl->vertices.emplace_front(3, glm::vec3{1,1,0}, c);
				Vertex *C = &mdl->vertices.front();
				mdl->vertices.emplace_front(4, glm::vec3{1,0,0}, d);
				Vertex *D = &mdl->vertices.front();
				mdl->vertices.emplace_front(5, glm::vec3{0,0,1}, e);
				Vertex *E = &mdl->vertices.front();
				mdl->vertices.emplace_front(6, glm::vec3{0,1,1}, f);
				Vertex *F = &mdl->vertices.front();
				mdl->vertices.emplace_front(7, glm::vec3{1,1,1}, g);
				Vertex *G = &mdl->vertices.front();
				mdl->vertices.emplace_front(8, glm::vec3{1,0,1}, h);
				Vertex *H = &mdl->vertices.front();

				mdl->loops.emplace_front(1,a);Loop *l1 = &mdl->loops.front();
				mdl->loops.emplace_front(2,l);Loop *l2 = &mdl->loops.front();
				mdl->loops.emplace_front(3,l);Loop *l3 = &mdl->loops.front();
				mdl->loops.emplace_front(4,i);Loop *l4 = &mdl->loops.front();
				mdl->loops.emplace_front(5,k);Loop *l5 = &mdl->loops.front();
				mdl->loops.emplace_front(6,e);Loop *l6 = &mdl->loops.front();

				#define set_e(v1,v2,v3,v4,v5,v6,v7,v8,v9) \
				v1->vstart  = v2 ;v1->vend    = v3;\
				v1->cwloop  = v4 ;v1->ccwloop = v5;\
				v1->cwpred  = v6 ;v1->cwsucc  = v7;\
				v1->ccwpred = v8 ;v1->ccwsucc = v9

				// loop 1
				set_e(a,A,B,l4,l1,i,j,d,b);
				set_e(b,B,C,l5,l1,k,i,a,c);
				set_e(c,C,D,l2,l1,l,k,b,d);
				set_e(d,D,A,l3,l1,j,l,c,a);

				// loop 2
				set_e(l,H,D,l3,l2,d,e,h,c);
				// [edge c]
				set_e(k,G,C,l2,l5,c,h,g,b);
				set_e(h,H,G,l2,l6,k,l,e,g);

				// loop 3
				// [edge l]
				set_e(e,E,H,l3,l6,l,j,f,h);
				set_e(j,A,E,l3,l4,e,d,a,f);
				// [edge d]

				// loop 4
				set_e(i,F,B,l5,l4,b,g,f,a);
				// [edge a]
				// [edge j]
				set_e(f,F,E,l4,l6,j,i,g,e);

				// loop 5
				// [edge k]
				// [edge b]
				// [edge i]
				set_e(g,G,F,l5,l6,i,k,h,f);

				// loop 6
				// [edge e]
				// [edge h]
				// [edge g]
				// [edge f]
			}

			ImGui::SameLine();
			if (ImGui::Button("Tetraedro")) {
				models.push_back(std::make_unique<tnw::BRep>(5,7,4));
				std::stringstream ss;
				ss << "BRep " << model_names.size() << "[TETRA]";
				model_names.push_back(ss.str());
				auto mdl = (tnw::BRep*)models.back().get();

				using namespace tnw::wed;
				mdl->edges.emplace_front(1);
				WEdge *a = &mdl->edges.front();
				mdl->edges.emplace_front(2);
				WEdge *b = &mdl->edges.front();
				mdl->edges.emplace_front(3);
				WEdge *c = &mdl->edges.front();
				mdl->edges.emplace_front(4);
				WEdge *d = &mdl->edges.front();
				mdl->edges.emplace_front(5);
				WEdge *e = &mdl->edges.front();
				mdl->edges.emplace_front(6);
				WEdge *f = &mdl->edges.front();
				mdl->vertices.emplace_front(1, glm::vec3{0,0,1}, a);
				Vertex *A = &mdl->vertices.front();
				mdl->vertices.emplace_front(2, glm::vec3{-1,0,0}, b);
				Vertex *B = &mdl->vertices.front();
				mdl->vertices.emplace_front(3, glm::vec3{1,0,0}, e);
				Vertex *C = &mdl->vertices.front();
				mdl->vertices.emplace_front(4, glm::vec3{0,1,0}, e);
				Vertex *D = &mdl->vertices.front();
				mdl->loops.emplace_front(1,c);
				Loop *l1 = &mdl->loops.front();
				mdl->loops.emplace_front(2,a);
				Loop *l2 = &mdl->loops.front();
				mdl->loops.emplace_front(3,b);
				Loop *l3 = &mdl->loops.front();
				mdl->loops.emplace_front(4,f);
				Loop *l4 = &mdl->loops.front();
				a->vstart = A;
				a->vend = D;
				a->cwloop = l3;
				a->ccwloop = l1;
				a->cwpred = e;
				a->cwsucc = f;
				a->ccwpred = b;
				a->ccwsucc = c;
				b->vstart = A;
				b->vend = B;
				b->cwloop = l1;
				b->ccwloop = l4;
				b->cwpred = c;
				b->cwsucc = a;
				b->ccwpred = f;
				b->ccwsucc = d;
				c->vstart = B;
				c->vend = D;
				c->cwloop = l1;
				c->ccwloop = l2;
				c->cwpred = a;
				c->cwsucc = b;
				c->ccwpred = d;
				c->ccwsucc = e;
				d->vstart = B;
				d->vend = C;
				d->cwloop = l2;
				d->ccwloop = l4;
				d->cwpred = e;
				d->cwsucc = c;
				d->ccwpred = b;
				d->ccwsucc = f;
				e->vstart = C;
				e->vend = D;
				e->cwloop = l2;
				e->ccwloop = l3;
				e->cwpred = c;
				e->cwsucc = d;
				e->ccwpred = f;
				e->ccwsucc = a;
				f->vstart = A;
				f->vend = C;
				f->cwloop = l4;
				f->ccwloop = l3;
				f->cwpred = d;
				f->cwsucc = b;
				f->ccwpred = a;
				f->ccwsucc = c;
			}
			ImGui::SameLine();
			if (ImGui::Button("Vazio")) {
				models.push_back(std::make_unique<tnw::BRep>());
				std::stringstream ss;
				ss << "BRep " << model_names.size();
				model_names.push_back(ss.str());
			}
		}
		ImGui::PopID();
	}

	if (ImGui::CollapsingHeader("Câmera")) {
		ImGui::InputFloat("Zoom", &camera.scale,camera.scale/2);
		ImGui::DragFloat("x##cam", &camera.pos.x,1/camera.scale*.01);
		ImGui::DragFloat("y##cam", &camera.pos.y,1/camera.scale*.01);
		ImGui::DragFloat("z##cam", &camera.pos.z,1/camera.scale*.01);
		if (ImGui::Button("CCW")) {
			rot = (rot-1)&3;
			camera.positive_hor = (rots[rot]>>0)&1;
			camera.positive_ver = (rots[rot]>>1)&1;
		}
		ImGui::SameLine();
		if (ImGui::Button("CW")) {
			rot = (rot+1)&3;
			camera.positive_hor = (rots[rot]>>0)&1;
			camera.positive_ver = (rots[rot]>>1)&1;
		}
		raycastCamera = camera;
	}

	if (ImGui::CollapsingHeader("Raycast")) {
		ImGui::InputInt("render width", &raycast_width);
		ImGui::InputInt("render height", &raycast_height);
		if (ImGui::Button("Render") && !render_show) {
			raycastCamera.aspect = float(raycast_height)/raycast_width;
			tnw::Raycast rc(models,raycastCamera,raycast_width,raycast_height,tex);
			render_show = true;
		}
	}

	ImGui::End();
}
