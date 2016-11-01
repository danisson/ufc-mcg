#include "csgtree.h"
#include "shapes.h"
#include <random>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <typeinfo>

using namespace tnw;
using std::array;
using std::unique_ptr;
using std::make_unique;
using std::move;
using std::get;
using std::make_tuple;

Color and_color(Color c1, Color c2) {
	switch (c1) {
		case Color::black: return c2;
		case Color::white: return Color::white;
		case Color::gray:
			if (c2 == Color::black) return Color::gray;
			else return c2;
	}
	return Color::white;
}

Color or_color(Color c1, Color c2) {
	switch (c1) {
		case Color::white: return c2;
		case Color::black: return Color::black;
		case Color::gray:
			if (c2 == Color::white) return Color::gray;
			else return c2;
	}
	return Color::white;
}

// Computa volume usando integração de Monte-Carlo
double tnw::csg::Node::volume() const {
	using        uniform          = std::uniform_real_distribution<double>;
	auto         bb               = boundingBox();

	auto         bounding_volume  = bb.volume();
	const size_t samples          = 10000;
	      size_t points_inside    = 0;
	const auto   min              = bb.minPoint();
	const auto   max              = bb.maxPoint();

	std::random_device r;
	std::default_random_engine gen(r());

	uniform dis[] = {uniform(min[0],max[0]),
	                 uniform(min[1],max[1]),
	                 uniform(min[2],max[2])};

	glm::vec3 test;
	for (size_t t = 0; t < samples; ++t) {
		for (size_t i = 0; i < 3; ++i)
			test[i] = dis[i](gen);

		if (intersect_point(test) != Color::white) points_inside++;
	}

	return bounding_volume * points_inside/double(samples);
}

//---------------------------------------------------------------------------//
tnw::csg::AndNode::AndNode(unique_ptr<Shape>&& a, unique_ptr<Shape>&& b) {
	children[0] = move(a);
	children[1] = move(b);
}

Color tnw::csg::AndNode::intersect_point(const glm::vec3& x) const {
	auto c1 = children[0]->intersect_point(x);
	auto c2 = children[1]->intersect_point(x);
	return and_color(c1,c2);
}

Color tnw::csg::AndNode::intersect_box(const BoundingBox& x) const {
	auto c1 = children[0]->intersect_box(x);
	auto c2 = children[1]->intersect_box(x);
	return and_color(c1,c2);
}

IntersectionList tnw::csg::AndNode::intersect_ray(const Ray& x) const {
	IntersectionList a = children[0]->intersect_ray(x);
	IntersectionList b = children[1]->intersect_ray(x);
	IntersectionList c;

	if (get<1>(b[0]) > get<1>(a[0]))
		std::swap(a,b);

	if (get<0>(a[0]) == Color::white) {
		c.push_back(a[0]);
		return c;
	}

	if (get<0>(b[0]) == Color::white) {
		c.push_back(b[0]);
		return c;
	}

	if (get<0>(b[0]) == get<0>(a[0])) {
		c.push_back(b[0]);
		return c;
	}

	if (get<0>(a[0]) == Color::gray) {
		c.push_back(make_tuple(Color::gray,get<1>(b[0])));
		return c;
	}

	if (get<0>(a[0]) == Color::black) {
		c.push_back(make_tuple(Color::gray,get<1>(b[0])));
		return c;
	}

	return c;
}

BoundingBox tnw::csg::AndNode::boundingBox() const {
	const auto b1 = children[0]->boundingBox();
	const auto b2 = children[1]->boundingBox();
	return b1.least_boundingbox(b2);
}

std::string tnw::csg::AndNode::serialize() const {
	// std::string s1 = children[0]->serialize(),
				// s2 = children[1]->serialize();
	// return s1.append(s2).append("AndNode");
	throw 0;
}

owner_ptr<Shape> tnw::csg::AndNode::clone() const {
	auto m1 = unique_ptr<Shape>(children[0]->clone());
	auto m2 = unique_ptr<Shape>(children[1]->clone());
	return new csg::AndNode(move(m1),move(m2));
}
//---------------------------------------------------------------------------//
tnw::csg::OrNode::OrNode(unique_ptr<Shape>&& a, unique_ptr<Shape>&& b) {
	children[0] = move(a);
	children[1] = move(b);
}

Color tnw::csg::OrNode::intersect_point(const glm::vec3& x) const {
	auto c1 = children[0]->intersect_point(x);
	auto c2 = children[1]->intersect_point(x);
	return or_color(c1,c2);
}

Color tnw::csg::OrNode::intersect_box(const BoundingBox& x) const {
	auto c1 = children[0]->intersect_box(x);
	auto c2 = children[1]->intersect_box(x);
	return or_color(c1,c2);
}

IntersectionList tnw::csg::OrNode::intersect_ray(const Ray& x) const {
	IntersectionList ilistA = children[0]->intersect_ray(x);
	IntersectionList ilistB = children[1]->intersect_ray(x);
	IntersectionList ilistC, ilistD;

	auto currA = ilistA[0];
	ilistA.erase(ilistA.begin());

	auto currB = ilistB[0];
	ilistB.erase(ilistB.begin());

	// std::tuple<tnw::Color, float> currC;
	float size;
	do {
		if (std::get<1>(currA) < std::get<1>(currB)) {
			size = std::get<1>(currA);
			auto currC = std::make_tuple(or_color(std::get<0>(currA), std::get<0>(currB)), size);
			ilistC.push_back(currC);

			std::get<1>(currB) -= size;
			if (!ilistA.empty()) {
				currA = ilistA[0];
				ilistA.erase(ilistA.begin());
			}

		} else if (std::get<1>(currB) < std::get<1>(currA)) {
			size = std::get<1>(currB);
			auto currC = std::make_tuple(or_color(std::get<0>(currA), std::get<0>(currB)), size);
			ilistC.push_back(currC);

			std::get<1>(currA) -= size;
			if (!ilistB.empty()) {
				currB = ilistB[0];
				ilistB.erase(ilistB.begin());
			}
		} else {
			size = std::get<1>(currA);
			auto currC = std::make_tuple(or_color(std::get<0>(currA), std::get<0>(currB)), size);
			ilistC.push_back(currC);

			if (!ilistA.empty() && !ilistB.empty()) {
				currA = ilistA[0];
				ilistA.erase(ilistA.begin());
				currB = ilistB[0];
				ilistB.erase(ilistB.begin());
			}
		}

	} while (!ilistA.empty() && !ilistB.empty());

	//Agora que tem a lista, tem que unir os elementos adjacentes da lista que tem a mesma cor
	float acumSize = 0;
	for (int i = 0; i < ilistC.size()-1; i++) {
		acumSize += std::get<1>(ilistC[i]);
		if (std::get<0>(ilistC[i+1]) != std::get<0>(ilistC[i])) {
			ilistD.push_back(std::make_tuple(std::get<0>(ilistC[i]), acumSize));
			acumSize = 0;
		}
	}
	ilistD.push_back(ilistC[ilistC.size()-1]);
	return ilistD;
}

BoundingBox tnw::csg::OrNode::boundingBox() const {
	const auto b1 = children[0]->boundingBox();
	const auto b2 = children[1]->boundingBox();
	return b1.least_boundingbox(b2);
}

std::string tnw::csg::OrNode::serialize() const {
	// std::string s1 = children[0]->serialize(),
		// s2 = children[1]->serialize();
		// return s1.append(s2).append("OrNode");
	throw 0;
}

owner_ptr<Shape> tnw::csg::OrNode::clone() const {
	auto m1 = unique_ptr<Shape>(children[0]->clone());
	auto m2 = unique_ptr<Shape>(children[1]->clone());
	return new csg::OrNode(move(m1),move(m2));
}
//---------------------------------------------------------------------------//
tnw::csg::ScaleNode::ScaleNode(unique_ptr<Shape>&& child, float dv)
: child(move(child)), dv(dv) {}

Color tnw::csg::ScaleNode::intersect_point(const glm::vec3& x) const {
	auto x2 = x*(1/dv);
	return child->intersect_point(x2);
}

Color tnw::csg::ScaleNode::intersect_box(const BoundingBox& x) const {
	auto x2 = x;
	x2.scale(1/dv);
	return child->intersect_box(x2);
}

IntersectionList tnw::csg::ScaleNode::intersect_ray(const Ray& x) const {
	auto transformedRay = x.getTransformedRay(glm::scale(glm::mat4(1.0f), {1.f/dv, 1.f/dv, 1.f/dv}));
	auto ilist = child->intersect_ray(transformedRay);
	for (auto&& x : ilist) {
		x = std::make_tuple(std::get<0>(x),std::get<1>(x)*dv);
	}
	return ilist;
}

BoundingBox tnw::csg::ScaleNode::boundingBox() const {
	auto b = child->boundingBox();
	b.scale(dv);
	return b;
}

std::string tnw::csg::ScaleNode::serialize() const {
	// std::string s1 = child->serialize();
		// return s1.append("ScaleNode");
	throw 0;
}

owner_ptr<Shape> tnw::csg::ScaleNode::clone() const {
	auto m1 = unique_ptr<Shape>(child->clone());
	return new csg::ScaleNode(move(m1),dv);
}
//---------------------------------------------------------------------------//
tnw::csg::TranslateNode::TranslateNode(unique_ptr<Shape>&& child, glm::vec3 dx)
: child(move(child)), dx(dx) {}

Color tnw::csg::TranslateNode::intersect_point(const glm::vec3& x) const {
	auto x2 = x-dx;
	return child->intersect_point(x2);
}

Color tnw::csg::TranslateNode::intersect_box(const BoundingBox& x) const {
	auto x2 = x;
	x2.translate(-dx);
	return child->intersect_box(x2);
}

IntersectionList tnw::csg::TranslateNode::intersect_ray(const Ray& x) const {
	Ray transformedRay = x.getTransformedRay(glm::translate(glm::mat4(1.0f), -dx));
	return child->intersect_ray(transformedRay);
}

BoundingBox tnw::csg::TranslateNode::boundingBox() const {
	auto b = child->boundingBox();
	b.translate(dx);
	return b;
}

std::string tnw::csg::TranslateNode::serialize() const {
	// std::string s1 = child->serialize();
		// return s1.append("TranslateNode");
	throw 0;
}

owner_ptr<Shape> tnw::csg::TranslateNode::clone() const {
	auto m1 = unique_ptr<Shape>(child->clone());
	return new csg::TranslateNode(move(m1),dx);
}
//---------------------------------------------------------------------------//
void tnw::CSGTree::rdraw() {
	if (should_update) {
		should_update = false;
		render_model = Octree(*root,this->boundingBox(),5);
		render_model.setColor(color);
	}

	render_model.draw();
}

tnw::CSGTree::CSGTree(unique_ptr<Shape>&& s) : root(new csg::ScaleNode(move(s),1)), render_model(BoundingBox({0,0,0},1)) {
	should_update = true;
	std::random_device r;
	std::default_random_engine gen(r());

	std::uniform_real_distribution<> dis(0,1);

	for (int i = 0; i < 3; ++i) {
		color[i] = dis(gen);
	}
}

tnw::CSGTree::CSGTree(owner_ptr<Shape> s) : CSGTree(unique_ptr<Shape>(s)) {}

tnw::CSGTree::CSGTree(owner_ptr<csg::Node> x) : root(x), render_model(BoundingBox({0,0,0},1)) {
	should_update = true;
	std::random_device r;
	std::default_random_engine gen(r());

	std::uniform_real_distribution<> dis(0,1);

	for (int i = 0; i < 3; ++i) {
		color[i] = dis(gen);
	}
}

tnw::CSGTree::CSGTree(FILE *f) : render_model(BoundingBox({0,0,0},1)) { 
	std::cout << "called\n";
	char c;
	bool eof = false;
	float cx,cy,cz,w,h,d,r,dx,dy,dz,ds;
	tnw::Shape* frontStack1, *frontStack2;
	// unique_ptr<tnw::Shape> frontStack1, frontStack2;
	// std::vector<unique_ptr<tnw::Shape>> stack;
	std::vector<tnw::Shape*> stack;
	while (!eof) {
		c = fgetc(f);
		if ((c == '\n') || (c == EOF)) {
			eof = true;
		} else {
			std::cout << "c: " << c << "\n";
			switch (c) {
				case 'B': {
					fscanf(f, "%f %f %f %f %f %f", &cx, &cy, &cz, &w, &h, &d);
					// auto bx =
					stack.push_back(new tnw::Box(glm::vec3(cx,cy,cz),w,h,d));
					break;
				}
				case 'S': {
					fscanf(f, "%f %f %f %f", &cx, &cy, &cz, &r);
					stack.push_back(new tnw::Sphere(glm::vec3(cx,cy,cz),r));
					break;
				}
				case 'C': {
					fscanf(f, "%f %f %f %f %f", &cx, &cy, &cz, &r, &h);
					stack.push_back(new tnw::Cilinder(glm::vec3(cx,cy,cz),h,r));
					break;
				}
				case 'O': {
					stack.push_back(new tnw::Octree(f));
					break;
				}
				case 'P': {
					fscanf(f, "%f %f %f %f %f", &cx, &cy, &cz, &r, &h);
					stack.push_back(new tnw::SquarePyramid(glm::vec3(cx,cy,cz),h,r));
					break;	
				}
				case 't': {
					fscanf(f, "%f %f %f", &dx, &dy, &dz);
					frontStack1 = std::move(stack.back());
					stack.pop_back();
					// stack.push_back(make_unique<tnw::csg::TranslateNode>(frontStack1, glm::vec3(dx,dy,dz)));
					stack.push_back(new tnw::csg::TranslateNode(unique_ptr<tnw::Shape>(frontStack1), glm::vec3(dx,dy,dz)));
					break;
				}
				case 's': {
					fscanf(f, "%f", &ds);
					frontStack1 = std::move(stack.back());
					stack.pop_back();
					stack.push_back(new tnw::csg::ScaleNode(unique_ptr<tnw::Shape>(frontStack1), ds));
					break;
				}
				case 'u': {
					frontStack1 = std::move(stack.back());
					stack.pop_back();
					frontStack2 = std::move(stack.back());
					stack.pop_back();
					stack.push_back(new tnw::csg::OrNode(unique_ptr<tnw::Shape>(frontStack1), unique_ptr<tnw::Shape>(frontStack2)));
					break;
				}
				case 'i': {
					frontStack1 = std::move(stack.back());
					stack.pop_back();
					frontStack2 = std::move(stack.back());
					stack.pop_back();
					stack.push_back(new tnw::csg::AndNode(unique_ptr<tnw::Shape>(frontStack1), unique_ptr<tnw::Shape>(frontStack2)));
					break;
				}
				case ' ': 
					break;
				//Não implementados
				case 'd':
				case 'r': {
					frontStack1 = std::move(stack.back());
					stack.push_back(new tnw::csg::TranslateNode(unique_ptr<tnw::Shape>(frontStack1),glm::vec3(0,0,0)));
					break;
				}
				default: {
					if (std::isupper(c)) {
						//Primitiva
						stack.push_back(new tnw::Box(glm::vec3(0,0,0),1,1,1));
					} else {
						//Transformação
						auto frontStack = std::move(stack.back());
						stack.push_back(new tnw::csg::TranslateNode(unique_ptr<tnw::Shape>(frontStack),glm::vec3(0,0,0)));
					}
					break;
				}			
			}
		}
	}

	// CSGTree(unique_ptr<Shape>(std::move(stack.back())));
	auto stackRoot = std::move(stack.back());
	auto nodeTree = new csg::ScaleNode(unique_ptr<Shape>(stackRoot), 1.0);
	root = unique_ptr<csg::Node>(nodeTree);

	should_update = true;
	std::random_device r2;
	std::default_random_engine gen(r2());

	std::uniform_real_distribution<> dis(0,1);

	for (int i = 0; i < 3; ++i) {
		color[i] = dis(gen);
	}
}

Color tnw::CSGTree::intersect_point(const glm::vec3& x) const {
	return root->intersect_point(x);
}

Color tnw::CSGTree::intersect_box(const BoundingBox& x) const {
	return root->intersect_box(x);
}

IntersectionList tnw::CSGTree::intersect_ray(const Ray& x) const {
	return root->intersect_ray(x);
}

BoundingBox tnw::CSGTree::boundingBox() const {
	return root->boundingBox();
}

double tnw::CSGTree::volume() const {
	return root->volume();
}

void tnw::CSGTree::translate(const glm::vec3& dx) {
	render_model.translate(dx);
	auto x = new csg::TranslateNode(move(root),dx);
	root.reset(x);
}
void tnw::CSGTree::scale(const float dv) {
	should_update = true;
	auto x = new csg::ScaleNode(move(root),dv);
	root.reset(x);
}

BooleanErrorCodes tnw::CSGTree::bool_and(const Model& y) {
	should_update = true;
	auto x = new csg::AndNode(move(root),unique_ptr<Shape>(y.clone()));
	root.reset(x);
	return BooleanErrorCodes::success;
}

BooleanErrorCodes tnw::CSGTree::bool_or(const Model& y) {
	should_update = true;
	auto x = new csg::OrNode(move(root),unique_ptr<Shape>(y.clone()));
	root.reset(x);
	return BooleanErrorCodes::success;
}

std::string tnw::CSGTree::serialize() const {
	return root->serialize();
}
void tnw::CSGTree::setColor(const float c[3]) {
	should_update = true;
	render_model.setColor(color);
	for (size_t i = 0; i < 3; ++i)
		color[i] = c[i];
}

PaintColor tnw::CSGTree::getColor() const {
	return {color[0],color[1],color[2]};
}

owner_ptr<Model> tnw::CSGTree::clone() const {
	return new CSGTree(root->clone());
}
