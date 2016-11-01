#include "interface.h"
#include "shapes.h"
#include "octree.h"
#include "csgtree.h"
#include <sstream>
#include <iostream>
#include <random>
#include "helper.h"
#include "raycast.h"

MainMenu::MainMenu(std::vector<std::unique_ptr<tnw::Model>>& m, IsometricCamera& c) : models(m), camera(c) {
	unsigned int m_size = m.size();
	for (unsigned int i = 0; i < m_size; i++) {
		std::stringstream ss;
		ss << "Árvore " << model_names.size();
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
		ImGui::InputInt("render width", &raycast_width);
		ImGui::InputInt("render height", &raycast_height);
		if (ImGui::Button("Render") && !render_show) {
			raycastCamera.aspect = float(raycast_height)/raycast_width;
			tnw::Raycast rc(models,raycastCamera,raycast_width,raycast_height,tex);
			render_show = true;
		}
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
			float halfWidth = ImGui::GetWindowContentRegionMax().x*.5f;
			float margin = ImGui::GetStyle().ItemSpacing.x;
			buttonSize.x = halfWidth-margin;
			if (ImGui::Button("Carregar",buttonSize)) {
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


		ImVec2 buttonSize = ImVec2(0,0);
		float fullWidth = ImGui::GetWindowContentRegionMax().x;
		float margin = ImGui::GetStyle().ItemSpacing.x;
		buttonSize.x = fullWidth/2-margin;

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


		buttonSize.x = fullWidth/3-margin;
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
					auto bb = new tnw::Box({x,y,z}, l, h, d);
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
	}

	ImGui::End();
}
