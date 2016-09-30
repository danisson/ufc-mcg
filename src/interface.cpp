#include "interface.h"

MainMenu::MainMenu(std::vector<std::unique_ptr<tnw::Model>>& m, IsometricCamera& c) : models(m), camera(c) {}

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
