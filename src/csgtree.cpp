#include "csgtree.h"
#include <random>

using namespace tnw;
using std::array;
using std::unique_ptr;
using std::make_unique;
using std::move;

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

IntersectionList tnw::csg::Node::intersect_ray(const Ray& ray) const {throw 0;}

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

BoundingBox tnw::csg::AndNode::boundingBox() const {
	const auto b1 = children[0]->boundingBox();
	const auto b2 = children[1]->boundingBox();
	return b1.least_boundingbox(b2);
}

std::string tnw::csg::AndNode::serialize() const {throw 0;}
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

BoundingBox tnw::csg::OrNode::boundingBox() const {
	const auto b1 = children[0]->boundingBox();
	const auto b2 = children[1]->boundingBox();
	return b1.least_boundingbox(b2);
}

std::string tnw::csg::OrNode::serialize() const {throw 0;}
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

BoundingBox tnw::csg::ScaleNode::boundingBox() const {
	auto b = child->boundingBox();
	b.scale(dv);
	return b;
}

std::string tnw::csg::ScaleNode::serialize() const {throw 0;}
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

BoundingBox tnw::csg::TranslateNode::boundingBox() const {
	auto b = child->boundingBox();
	b.translate(dx);
	return b;
}

std::string tnw::csg::TranslateNode::serialize() const {throw 0;}
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

Color tnw::CSGTree::intersect_point(const glm::vec3& x) const {
	return root->intersect_point(x);
}

Color tnw::CSGTree::intersect_box(const BoundingBox& x) const {
	return root->intersect_box(x);
}

IntersectionList tnw::CSGTree::intersect_ray(const Ray& ray) const {throw 0;}

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
	render_model.setColor(color);
	for (size_t i = 0; i < 3; ++i)
		color[i] = c[i];
}

PaintColor tnw::CSGTree::getColor() const {throw 0;}

owner_ptr<Model> tnw::CSGTree::clone() const {throw 0;}
