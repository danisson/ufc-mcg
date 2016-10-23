#include "csgtree.h"

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

std::string tnw::csg::TranslateNode::serialize() const {throw 0;}
//---------------------------------------------------------------------------//
void tnw::CSGTree::rdraw() const {throw 0;}

Color tnw::CSGTree::intersect_point(const glm::vec3& x) const {
	return root->intersect_point(x);
}
Color tnw::CSGTree::intersect_box(const BoundingBox& x) const {
	return root->intersect_box(x);
}

double tnw::CSGTree::volume() const {
	return root->volume();
}

void tnw::CSGTree::translate(const glm::vec3& dx) {
	auto x = new csg::TranslateNode(move(root),dx);
	root.reset(x);
}
void tnw::CSGTree::scale(const float dv) {
	auto x = new csg::ScaleNode(move(root),dv);
	root.reset(x);
}

BooleanErrorCodes tnw::CSGTree::bool_and(const Model& y) {
	auto x = new csg::AndNode(move(root),unique_ptr<Shape>(y.clone()));
	root.reset(x);
	return BooleanErrorCodes::success;
}

BooleanErrorCodes tnw::CSGTree::bool_or(const Model& y) {
	auto x = new csg::OrNode(move(root),unique_ptr<Shape>(y.clone()));
	root.reset(x);
	return BooleanErrorCodes::success;
}

std::string tnw::CSGTree::serialize() const {
	return root->serialize();
}
void tnw::CSGTree::setColor(float c[3]) {
	for (size_t i = 0; i < 3; ++i)
		color[i] = c[i];
}
