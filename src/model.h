#ifndef _H_MODEL
#define _H_MODEL
#include <glm/glm.hpp>

namespace tnw {

class Model {
public:
	// Função que será usada pra desenhar o modelo
	virtual void draw() = 0;
	// Funções geométricas
	virtual void translate(const glm::vec3& dv);
	// Operações booleanas
	virtual void bool_and(const Model& y);
	// Análise geométrica
	virtual double volume() = 0;
};


using Classificador = std::function<bool(glm::mat3x4)>;
class Octree : public Model {
public:
	Octree();
	~Octree();
};

} // namespace tnw
#endif
