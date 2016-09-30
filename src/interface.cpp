#include "interface.h"
#include <sstream>
#include <iostream>
#include <random>

MainMenu::MainMenu(std::vector<std::unique_ptr<tnw::Model>>& m, IsometricCamera& c) : models(m), camera(c) {
	unsigned int m_size = m.size();
	for (unsigned int i = 0; i < m_size; i++) {
		std::stringstream ss;
		ss << "Árvore " << model_names.size();
		model_names.push_back(ss.str());
	}
}

void MainMenu::draw() {
	ImGui::Begin("Menu");
	
	if (ImGui::CollapsingHeader("Arquivo")) {
		if (ImGui::Button("Abrir")) {

		}
		ImGui::SameLine();
		if (ImGui::Button("Salvar")) {

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

		if (ImGui::Button("Translação") && (curr_item >= 0 && static_cast<unsigned int>(curr_item) < models.size())) {
			ImGui::OpenPopup("Parâmetros da Translação");
		}

		if (ImGui::BeginPopupModal("Parâmetros da Translação", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::InputFloat("x", &x);
			ImGui::InputFloat("y", &y);
			ImGui::InputFloat("z", &z);

			if (ImGui::Button("OK", ImVec2(120,0))) {
				if (curr_item >= 0 && static_cast<unsigned int>(curr_item) < models.size()) {
					models[curr_item]->translate(glm::vec3(x,y,z));
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Remover") && (curr_item >= 0 && static_cast<unsigned int>(curr_item) < models.size())) {
			models.erase(models.begin() + curr_item);
			model_names.erase(model_names.begin() + curr_item);
		}

		ImGui::SameLine();
		if (ImGui::Button("União") && (curr_item >= 0 && static_cast<unsigned int>(curr_item) < models.size())) {
			ImGui::OpenPopup("Parâmetros de União");
		}

		if (ImGui::BeginPopupModal("Parâmetros de União", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			static int selected_or = -1;
			ImGui::Combo("selecione a árvore com que operar", &selected_or, tree_names, model_names.size());
			
			
			if (ImGui::Button("OK", ImVec2(120,0)) && (selected_or >= 0 && static_cast<unsigned int>(selected_or) < models.size())) {
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
		if (ImGui::Button("Interseção") && (curr_item >= 0 && static_cast<unsigned int>(curr_item) < models.size())) {
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

		ImGui::SameLine();
		if (ImGui::Button("Cor") && (curr_item >= 0 && static_cast<unsigned int>(curr_item) < models.size())) {
			ImGui::OpenPopup("Parâmetros de Cor");
		}

		if (ImGui::BeginPopupModal("Parâmetros de Cor", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			
			ImGui::ColorEdit3("cor", color);
			if (ImGui::Button("Cor aleatória")) {
				std::random_device r;
				std::default_random_engine gen(r());

				std::uniform_real_distribution<> dis(0,1);

				for (int i = 0; i < 3; ++i)
				{
					color[i] = dis(gen);
				}
			}
			if (ImGui::Button("OK", ImVec2(120,0))) {
				models[curr_item]->setColor(color);
				ImGui::CloseCurrentPopup(); 
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}			

		if (ImGui::CollapsingHeader("Nova Primitiva")) {
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
					tnw::octree::BoundingBox b({bx,by,bz},bd);
					tnw::octree::Sphere s({x,y,z}, r);
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
					tnw::octree::Box bb({x,y,z}, l, h, d);
					tnw::octree::BoundingBox b({bx,by,bz},bd);
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
					tnw::octree::Cilinder cl(glm::vec3(x,y,z), h, r);
					tnw::octree::BoundingBox b({bx,by,bz},bd);
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
					tnw::octree::SquarePyramid sp({x,y,z}, h, l);
					tnw::octree::BoundingBox b({bx,by,bz},bd);
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
		}
	}

	if (ImGui::CollapsingHeader("Câmera")) {
		ImGui::InputFloat("Zoom", &camera.scale,.5,1);
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
	}

	ImGui::End();
}
