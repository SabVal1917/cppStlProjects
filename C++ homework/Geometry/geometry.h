#include <cmath>
#include <iostream>
#include <vector>

const double kComparisonAccuracy = 1e-6;

bool IsEqualTwoDouble(double first, double second) {
  return (std::abs(first - second) < kComparisonAccuracy);
}

struct Point;
struct Vector;
struct Line;

struct Point {
  double x;
  double y;
  Point() = default;
  Point(double x, double y);
  Point(const Point& other);
  Point& operator=(const Point& other);
  void rotate(const Point& center, double angle);
  void reflect(const Point& center);
  void reflect(const Line& axis);
  void scale(const Point& center, double coefficient);
};

struct Vector {
  double x;
  double y;
  Vector() = default;
  Vector(const Point& first, const Point& second);
  Vector(double first, double second);
  Vector(const Vector& other);
  Vector& operator=(const Vector& other);
  double VctrLen() const;
  void Normalize();
  void ScaleVect(double alpha);
  Point GetEndPoint(const Point& other);
};

struct Line {
  double A;
  double B;
  double C;
  Line() = default;
  Line(const Point& first, const Point& second);
  Line(double k, double b);
  Line(const Point& point, double k);
  double GetValueOfEquation(const Point& point) const;
  bool PointInLine(const Point& point) const;
  Vector GetDirectingVector() const;
  Vector GetNormalVector() const;
  std::vector<double> GetKoef() const;
};

bool operator==(const Point& first, const Point& second);
bool operator!=(const Point& first, const Point& second);

bool operator==(const Vector& first, const Vector& second);
bool operator!=(const Vector& first, const Vector& second);

bool operator==(const Line& first, const Line& second);
bool operator!=(const Line& first, const Line& second);

double GetVectMultiple(const Vector& first, const Vector& second) {
  return first.x * second.y - second.x * first.y;
}

double GetScaleMultiple(const Vector& first, const Vector& second) {
  return first.x * second.x + first.y * second.y;
}

double GetAngleBetweenVectors(const Vector& first, const Vector& second) {
  return acos(GetScaleMultiple(first, second) /
              (first.VctrLen() * second.VctrLen()));
}

void Point::rotate(const Point& center, double angle) {
  double old_x = x;
  x = center.x + cos(angle) * (x - center.x) - sin(angle) * (y - center.y);
  y = center.y + sin(angle) * (old_x - center.x) + cos(angle) * (y - center.y);
}

double GetDistPointLine(const Line& line, const Point& point) {
  const std::vector<double> koef = line.GetKoef();
  return std::abs(point.x * koef[0] + point.y * koef[1] + koef[2]) /
         sqrt(koef[0] * koef[0] + koef[1] * koef[1]);
}

Point GetPrOnLine(const Line& line, const Point& point) {
  Vector normal = line.GetNormalVector();
  normal.Normalize();
  normal.ScaleVect(GetDistPointLine(line, point));
  Vector reverse_normal = normal;
  reverse_normal.ScaleVect(-1.0);
  Point first_candidate = normal.GetEndPoint(point);
  Point second_candidate = reverse_normal.GetEndPoint(point);
  if (GetDistPointLine(line, first_candidate) <
      GetDistPointLine(line, second_candidate)) {
    return first_candidate;
  }
  return second_candidate;
}

void Point::reflect(const Point& center) {
  Vector directing_vect(center, *this);
  *this = directing_vect.GetEndPoint(center);
}

void Point::reflect(const Line& axis) {
  Point prj_point = GetPrOnLine(axis, *this);
  reflect(prj_point);
}

void Point::scale(const Point& center, double coefficient) {
  Vector direct_vect(*this, center);
  direct_vect.ScaleVect(coefficient);
  *this = direct_vect.GetEndPoint(center);
}

Point::Point(double x, double y) : x(x), y(y) {}
Point::Point(const Point& other) : x(other.x), y(other.y) {}

Point& Point::operator=(const Point& other) {
  x = other.x;
  y = other.y;
  return *this;
}

bool operator==(const Point& first, const Point& second) {
  return (IsEqualTwoDouble(first.x, second.x) &&
          IsEqualTwoDouble(first.y, second.y));
}
bool operator!=(const Point& first, const Point& second) {
  return !(first == second);
}
Point GetMidOfSeqment(const Point& first, const Point& second) {
  return Point((first.x + second.x) / 2.0, (first.y + second.y) / 2.0);
}

// Vector definitions
double Vector::VctrLen() const { return sqrt(x * x + y * y); }
void Vector::Normalize() {
  double v_len = VctrLen();
  x /= v_len;
  y /= v_len;
}
void Vector::ScaleVect(double alpha) {
  x *= alpha;
  y *= alpha;
}
Point Vector::GetEndPoint(const Point& other) {
  return Point(other.x + x, other.y + y);
}

Vector::Vector(const Point& first, const Point& second)
    : x(first.x - second.x), y(first.y - second.y) {}
Vector::Vector(double first, double second) : x(first), y(second) {}
Vector::Vector(const Vector& other) : x(other.x), y(other.y) {}
Vector& Vector::operator=(const Vector& other) {
  x = other.x;
  y = other.y;
  return *this;
}

bool operator==(const Vector& first, const Vector& second) {
  return (IsEqualTwoDouble(first.x, second.x) &&
          IsEqualTwoDouble(first.y, second.y));
}
bool operator!=(const Vector& first, const Vector& second) {
  return !(first == second);
}

double GetDistBetweenPoints(const Point& first, const Point& second) {
  Vector vctr(first, second);
  return vctr.VctrLen();
}

double Line::GetValueOfEquation(const Point& point) const {
  return A * point.x + B * point.y + C;
}
bool Line::PointInLine(const Point& point) const {
  return IsEqualTwoDouble(GetValueOfEquation(point), 0.0);
}
Vector Line::GetDirectingVector() const { return Vector(-B, A); }
Vector Line::GetNormalVector() const { return Vector(A, B); }
std::vector<double> Line::GetKoef() const {
  std::vector<double> koef = {A, B, C};
  return koef;
}
Line::Line(const Point& first, const Point& second) {
  Vector directing_vect(first, second);
  A = directing_vect.y;
  B = -directing_vect.x;
  C = directing_vect.x * first.y - directing_vect.y * first.x;
}
Line::Line(double k, double b) : A(k), B(-1.0), C(b) {}
Line::Line(const Point& point, double k)
    : A(k), B(-1.0), C(point.y - k * point.x) {}

bool operator==(const Line& first, const Line& second) {
  if ((IsEqualTwoDouble(first.B, 0) && IsEqualTwoDouble(second.B, 0)) &&
      (!IsEqualTwoDouble(first.A, 0) && !IsEqualTwoDouble(second.A, 0))) {
    return IsEqualTwoDouble(-first.C / first.A, -second.C / second.A);
  }
  if ((IsEqualTwoDouble(first.B, 0) && IsEqualTwoDouble(second.B, 0)) &&
      (IsEqualTwoDouble(first.A, 0) && IsEqualTwoDouble(second.A, 0))) {
    return IsEqualTwoDouble(first.C, second.C);
  }
  Point first_point(0.0, -first.C / first.B);
  Point second_point(1.0, (-first.C - first.A) / first.B);
  return (second.PointInLine(first_point) && second.PointInLine(second_point));
}
bool operator!=(const Line& first, const Line& second) {
  return !(first == second);
};

void MovePointAlongLineForDistance(Point& point, const Line& line,
                                   double distance) {
  Vector direct_vect = line.GetDirectingVector();
  direct_vect.Normalize();
  direct_vect.ScaleVect(distance);
  point = direct_vect.GetEndPoint(point);
}

Line GetBisectBAC(const Point& A, const Point& B, const Point& C) {
  Vector vect_BA(B, A);
  Vector vect_CA(C, A);
  vect_BA.Normalize();
  vect_CA.Normalize();
  Point bisect_point(A.x + (vect_CA.x + vect_BA.x) / 2.0,
                     A.y + (vect_CA.y + vect_BA.y) / 2.0);
  return Line(A, bisect_point);
}

class Shape;
class Polygon;
class Rectangle;
class Square;
class Triangle;
class Ellipse;
class Circle;

class Shape {
 public:
  virtual double perimeter() const = 0;
  virtual double area() const = 0;

  virtual bool isEqualTo(const Shape& another) const = 0;
  virtual bool isCongruentTo(const Shape& another) const = 0;
  virtual bool isSimilarTo(const Shape& another) const = 0;
  virtual bool containsPoint(const Point& point) const = 0;
  virtual void rotate(const Point& center, double angle) = 0;
  virtual void reflect(const Point& center) = 0;
  virtual void reflect(const Line& axis) = 0;
  virtual void scale(const Point& center, double coefficient) = 0;

  virtual ~Shape() = default;
};

bool operator==(const Shape& first, const Shape& second) {
  return first.isEqualTo(second);
}

class Polygon : public Shape {
 protected:
  std::vector<Point> points_;

 public:
  template <typename... T>
  Polygon(T... features);
  Polygon(std::vector<Point> points);
  size_t verticesCount() const;
  const std::vector<Point>& getVertices() const;
  bool isConvex();
  virtual double perimeter() const override;
  virtual double area() const override;
  virtual bool isEqualTo(const Shape& another) const override;
  virtual bool isCongruentTo(const Shape& another) const override;
  virtual bool isSimilarTo(const Shape& another) const override;
  virtual bool containsPoint(const Point& point) const override;

  virtual void rotate(const Point& center, double angle) override;
  virtual void reflect(const Point& center) override;
  virtual void reflect(const Line& axis) override;
  virtual void scale(const Point& center, double coefficient) override;
};

class Rectangle : public Polygon {
 public:
  Rectangle(const Point& first_pt, const Point& second_pt, double coef);
  Point center();
  std::pair<Line, Line> diagonals();
};

class Square : public Rectangle {
 public:
  Square(const Point& frst, const Point& scnd);
  Circle circumscribedCircle();
  Circle inscribedCircle();
};

class Triangle : public Polygon {
  using Polygon::Polygon;

 public:
  Circle circumscribedCircle() const;
  Circle inscribedCircle() const;
  Point centroid() const;
  Point orthocenter() const;
  Line EulerLine() const;
  Circle ninePointsCircle() const;
};

class Ellipse : public Shape {
 protected:
  Point focus1_;
  Point focus2_;
  double distance_;

 public:
  Ellipse() = default;
  Ellipse(const Point& focus1, const Point& focus2, double distance);
  Ellipse(const Ellipse& another);
  Ellipse& operator=(const Ellipse& another);

  void build_directrice(Line& directrice, bool direction) const;

  std::pair<Point, Point> focuses() const;

  std::pair<Line, Line> directrices() const;

  double GetLargeAxis() const;
  double GetSmallAxis() const;
  double eccentricity() const;
  Point center() const;

  virtual double perimeter() const override;
  virtual double area() const override;
  virtual bool isEqualTo(const Shape& another) const override;
  virtual bool isCongruentTo(const Shape& another) const override;
  virtual bool isSimilarTo(const Shape& another) const override;
  virtual bool containsPoint(const Point& point) const override;

  virtual void rotate(const Point& center, double angle) override;
  virtual void reflect(const Point& center) override;
  virtual void reflect(const Line& axis) override;
  virtual void scale(const Point& center, double coefficient) override;
};

class Circle : public Ellipse {
 public:
  Circle(const Point& point, double radius);
  double radius() const;
};

double Polygon::perimeter() const {
  double result = 0;
  for (size_t i = 0; i < points_.size(); ++i) {
    result +=
        GetDistBetweenPoints(points_[i], points_[(i + 1) % points_.size()]);
  }
  return result;
}
double Polygon::area() const {
  double result = 0;
  for (size_t i = 0; i < points_.size(); ++i) {
    result += points_[i].x * points_[(i + 1) % points_.size()].y -
              points_[i].y * points_[(i + 1) % points_.size()].x;
  }
  return std::abs(result) / 2.0;
}

bool IsEqualInDirection(const std::vector<Point>& first,
                        const std::vector<Point>& second, int index,
                        bool is_clockwise) {
  int vec_len = static_cast<int>(first.size());
  for (int i = 0; i < vec_len; ++i) {
    if (is_clockwise && (first[i] != second[(index + i) % vec_len])) {
      return false;
    }
    if (!is_clockwise &&
        (first[i] != second[(vec_len - i + index) % vec_len])) {
      return false;
    }
  }
  return true;
}

double GetVertexAngleInPolygon(const std::vector<Point>& first, size_t index) {
  Vector first_vector(first[(index + 1) % first.size()], first[index]);
  Vector second_vector(first[(first.size() + index - 1) % first.size()],
                       first[index]);
  double angle = GetAngleBetweenVectors(first_vector, second_vector);
  return angle;
}

double IsSimilarInDirection(const std::vector<Point>& first,
                            const std::vector<Point>& second, size_t index,
                            bool is_clockwise) {
  size_t vec_len = first.size();
  double result;
  for (size_t i = 0; i < vec_len; ++i) {
    if (is_clockwise) {
      double first_angle = GetVertexAngleInPolygon(first, i);
      double second_angle =
          GetVertexAngleInPolygon(second, (index + i) % vec_len);
      if (!IsEqualTwoDouble(first_angle, second_angle)) {
        return 0;
      }
    }
    if (!is_clockwise) {
      double first_angle = GetVertexAngleInPolygon(first, i);
      double second_angle =
          GetVertexAngleInPolygon(second, (vec_len - i + index) % vec_len);
      if (!IsEqualTwoDouble(first_angle, second_angle)) {
        return 0;
      }
    }
  }
  if (is_clockwise) {
    result = (Vector(first[0], first[1]).VctrLen()) /
             (Vector(second[index], second[(index + 1) % vec_len]).VctrLen());
  } else {
    result = (Vector(first[0], first[1]).VctrLen()) /
             (Vector(second[(vec_len - 1 + index) % vec_len],
                     second[(vec_len - 0 + index) % vec_len])
                  .VctrLen());
  }
  return result;
}

double GetSimilarityCoefficient(const std::vector<Point>& first,
                                const std::vector<Point>& second) {
  double result = 0;
  double zero_angle = GetVertexAngleInPolygon(first, 0);
  for (size_t i = 0; i < second.size(); ++i) {
    double current_angle = GetVertexAngleInPolygon(second, i);
    if (IsEqualTwoDouble(current_angle, zero_angle)) {
      double result_1 = IsSimilarInDirection(first, second, i, true);
      double result_2 = IsSimilarInDirection(first, second, i, false);
      if (!IsEqualTwoDouble(result_1, 0.0)) {
        return result_1;
      }
      if (!IsEqualTwoDouble(result_2, 0.0)) {
        return result_2;
      }
    }
  }
  return result;
}

bool Polygon::isEqualTo(const Shape& another) const {
  const Polygon* another_polygon = dynamic_cast<const Polygon*>(&another);
  if (another_polygon == nullptr) {
    return false;
  }
  if (another_polygon->verticesCount() != points_.size()) {
    return false;
  }
  int index_of_matching_points = -1;
  std::vector<Point> another_vertex = another_polygon->getVertices();
  for (int i = 0; i < static_cast<int>(another_vertex.size()); ++i) {
    if (another_vertex[i] == points_[0]) {
      index_of_matching_points = i;
      break;
    }
  }
  if (index_of_matching_points == -1) {
    return false;
  }
  return IsEqualInDirection(points_, another_vertex, index_of_matching_points,
                            true) ||
         IsEqualInDirection(points_, another_vertex, index_of_matching_points,
                            false);
}

bool Polygon::isCongruentTo(const Shape& another) const {
  const Polygon* another_polygon = dynamic_cast<const Polygon*>(&another);
  if (another_polygon == nullptr) {
    return false;
  }
  if (another_polygon->verticesCount() != points_.size()) {
    return false;
  }
  return IsEqualTwoDouble(
      GetSimilarityCoefficient(points_, another_polygon->points_), 1.0);
}

bool Polygon::isSimilarTo(const Shape& another) const {
  const Polygon* another_polygon = dynamic_cast<const Polygon*>(&another);
  if (another_polygon == nullptr) {
    return false;
  }
  if (another_polygon->verticesCount() != points_.size()) {
    return false;
  }
  return !IsEqualTwoDouble(
      GetSimilarityCoefficient(points_, another_polygon->points_), 0.0);
}

Point IntersectLines(const Line& first, const Line& second) {
  return Point((second.C * first.B - second.B * first.C) /
                   (first.A * second.B - second.A * first.B),
               (second.A * first.C - second.C * first.A) /
                   (first.A * second.B - second.A * first.B));
}

bool is_point_in_segment(Point p1, Point p2, Point p3) {
  Vector vc21(p1, p2);
  Vector vc23(p3, p2);
  Vector vc32(p2, p3);
  Vector vc31(p1, p3);
  return (IsEqualTwoDouble(GetVectMultiple(vc21, vc23), 0.0)) &&
         (GetScaleMultiple(vc21, vc23) >= 0) &&
         (GetScaleMultiple(vc32, vc31) >= 0);
}

bool RayContainsPoint(const Point& point, const Point& begin_ray,
                      const Point& end_ray) {
  Vector vector_21(begin_ray, point);
  Vector vector_32(begin_ray, end_ray);
  return IsEqualTwoDouble(GetVectMultiple(vector_21, vector_32), 0.0) &&
         GetScaleMultiple(vector_21, vector_32) >= 0;
}

bool Polygon::containsPoint(const Point& point) const {
  Point end_ray = point;
  end_ray.x -= 10;
  Line ray(point, end_ray);
  int counter = 0;
  for (size_t i = 0; i < points_.size(); ++i) {
    Line segment(points_[i], points_[(i + 1) % points_.size()]);
    Point intersection_point = IntersectLines(ray, segment);
    if (RayContainsPoint(intersection_point, point, end_ray)) {
      if (is_point_in_segment(intersection_point, points_[i],
                              points_[(i + 1) % points_.size()])) {
        ++counter;
      }
    }
  }
  //  std::cerr << (counter % 2 == 1) << '\n';
  //  std::cerr << '\n' << '\n' << '\n';
  return counter % 2 == 1;
}

void Polygon::rotate(const Point& center, double angle) {
  for (size_t i = 0; i < points_.size(); ++i) {
    points_[i].rotate(center, angle);
  }
}
void Polygon::reflect(const Point& center) {
  for (size_t i = 0; i < points_.size(); ++i) {
    points_[i].reflect(center);
  }
}
void Polygon::reflect(const Line& axis) {
  for (size_t i = 0; i < points_.size(); ++i) {
    points_[i].reflect(axis);
  }
}
void Polygon::scale(const Point& center, double coefficient) {
  for (size_t i = 0; i < points_.size(); ++i) {
    points_[i].scale(center, coefficient);
  }
}
bool Polygon::isConvex() {
  if (points_.size() < 4) {
    return true;
  }
  bool sign = false;
  size_t size_pts = points_.size();
  for (int i = 0; i < static_cast<int>(size_pts); i++) {
    Vector vect1(points_[(i + 2) % size_pts], points_[(i + 1) % size_pts]);
    Vector vect2(points_[(i) % size_pts], points_[(i + 1) % size_pts]);
    double zcrossproduct = vect1.x * vect2.y - vect1.y * vect2.x;
    if (i == 0) {
      sign = zcrossproduct > 0;
    } else if (sign != (zcrossproduct > 0)) {
      return false;
    }
  }
  return true;
}

Polygon::Polygon(std::vector<Point> points) : points_(points) {}

size_t Polygon::verticesCount() const { return points_.size(); }

const std::vector<Point>& Polygon::getVertices() const { return points_; }

template <typename... T>
Polygon::Polygon(T... features) : points_({features...}) {}

bool Ellipse::isEqualTo(const Shape& another) const {
  const Ellipse* another_ellipse = dynamic_cast<const Ellipse*>(&another);
  if (another_ellipse == nullptr) {
    return false;
  }
  if ((another_ellipse->focus1_ == this->focus1_ &&
       another_ellipse->focus2_ == this->focus2_) ||
      (another_ellipse->focus1_ == this->focus2_ &&
       another_ellipse->focus2_ == this->focus1_)) {
    return IsEqualTwoDouble(this->distance_, another_ellipse->distance_);
  }
  return false;
}
bool Ellipse::isCongruentTo(const Shape& another) const {
  const Ellipse* another_ellipse = dynamic_cast<const Ellipse*>(&another);
  if (another_ellipse == nullptr) {
    return false;
  }
  return IsEqualTwoDouble(this->eccentricity(),
                          another_ellipse->eccentricity()) &&
         IsEqualTwoDouble(this->distance_, another_ellipse->distance_);
}
bool Ellipse::isSimilarTo(const Shape& another) const {
  const Ellipse* another_ellipse = dynamic_cast<const Ellipse*>(&another);
  if (another_ellipse == nullptr) {
    return false;
  }
  return IsEqualTwoDouble(this->eccentricity(),
                          another_ellipse->eccentricity());
}
bool Ellipse::containsPoint(const Point& point) const {
  std::cerr << ((point.x * point.x / (GetLargeAxis() * GetLargeAxis()) +
                 point.y * point.y / (GetSmallAxis() * GetSmallAxis())) <= 1.0)
            << '\n';
  return ((point.x - center().x) * (point.x - center().x) /
              (GetLargeAxis() * GetLargeAxis()) +
          (point.y - center().y) * (point.y - center().y) /
              (GetSmallAxis() * GetSmallAxis())) <= 1.0;
}

void Ellipse::rotate(const Point& center, double angle) {
  focus1_.rotate(center, angle);
  focus2_.rotate(center, angle);
}
void Ellipse::reflect(const Point& center) {
  focus1_.reflect(center);
  focus2_.reflect(center);
}
void Ellipse::reflect(const Line& axis) {
  focus1_.reflect(axis);
  focus2_.reflect(axis);
}
void Ellipse::scale(const Point& center, double coefficient) {
  focus1_.scale(center, coefficient);
  focus2_.scale(center, coefficient);
  distance_ *= std::abs(coefficient);
}

void Ellipse::build_directrice(Line& directrice, bool direction) const {
  Line focus_line(focus1_, focus2_);
  Point intersection_point = center();
  double distance = GetLargeAxis() / eccentricity() * (direction ? 1.0 : -1.0);
  MovePointAlongLineForDistance(intersection_point, focus_line, distance);
  Vector normal_vect = focus_line.GetNormalVector();
  Point second_pt = normal_vect.GetEndPoint(intersection_point);
  directrice = Line(intersection_point, second_pt);
}
std::pair<Line, Line> Ellipse::directrices() const {
  Line directrice1;
  Line directrice2;
  build_directrice(directrice1, true);
  build_directrice(directrice2, false);
  return {directrice1, directrice2};
}
double Ellipse::eccentricity() const {
  double focus_dist = GetDistBetweenPoints(focus1_, focus2_) / 2.0;
  return focus_dist / GetLargeAxis();
}
Point Ellipse::center() const { return GetMidOfSeqment(focus1_, focus2_); }
double Ellipse::perimeter() const {
  double exc = eccentricity();
  return 4 * GetLargeAxis() * std::comp_ellint_2(exc);
}
double Ellipse::area() const { return M_PI * GetLargeAxis() * GetSmallAxis(); }
Ellipse::Ellipse(const Point& focus1, const Point& focus2, double distance)
    : focus1_(focus1), focus2_(focus2), distance_(distance) {}
Ellipse::Ellipse(const Ellipse& another)
    : focus1_(another.focus1_),
      focus2_(another.focus2_),
      distance_(another.distance_) {}
Ellipse& Ellipse::operator=(const Ellipse& another) {
  focus1_ = another.focus1_;
  focus2_ = another.focus2_;
  distance_ = another.distance_;
  return *this;
}
std::pair<Point, Point> Ellipse::focuses() const { return {focus1_, focus2_}; }
double Ellipse::GetLargeAxis() const { return distance_ / 2.0; }
double Ellipse::GetSmallAxis() const {
  double focus_dist = GetDistBetweenPoints(focus1_, focus2_) / 2.0;
  return sqrt(GetLargeAxis() * GetLargeAxis() - focus_dist * focus_dist);
}

Circle::Circle(const Point& point, double radius)
    : Ellipse(point, point, radius) {}
double Circle::radius() const { return distance_; }

Rectangle::Rectangle(const Point& first_pt, const Point& second_pt,
                     double coef) {
  std::vector<Point> polygon;
  polygon.push_back(first_pt);
  Point center = GetMidOfSeqment(first_pt, second_pt);
  Point copy_point = first_pt;
  copy_point.rotate(center, M_PI + 2 * atan(coef));
  polygon.push_back(copy_point);
  polygon.push_back(second_pt);
  copy_point.reflect(center);
  polygon.push_back(copy_point);
  points_ = polygon;
}
Point Rectangle::center() { return GetMidOfSeqment(points_[0], points_[2]); }
std::pair<Line, Line> Rectangle::diagonals() {
  return {Line(points_[0], points_[2]), Line(points_[1], points_[3])};
}

Square::Square(const Point& frst, const Point& scnd)
    : Rectangle(frst, scnd, 1) {}
Circle Square::circumscribedCircle() {
  return Circle(center(), GetDistBetweenPoints(center(), points_[0]));
}
Circle Square::inscribedCircle() {
  return Circle(center(), GetDistBetweenPoints(points_[0], points_[1]) / 2.0);
}

Line GetSerPer(const Point& first, const Point& second) {
  Line first_line(first, second);
  Point mid_of_first = GetMidOfSeqment(first, second);
  Vector normal_first = first_line.GetNormalVector();
  Point second_point_of_first = normal_first.GetEndPoint(mid_of_first);
  Line first_ser_per(mid_of_first, second_point_of_first);
  return first_ser_per;
}

Circle Triangle::circumscribedCircle() const {
  Point center = IntersectLines(GetSerPer(points_[0], points_[1]),
                                GetSerPer(points_[0], points_[2]));
  double radius = GetDistBetweenPoints(center, points_[0]);
  return Circle(center, radius);
}

Circle Triangle::inscribedCircle() const {
  Point center =
      IntersectLines(GetBisectBAC(points_[1], points_[0], points_[2]),
                     GetBisectBAC(points_[0], points_[1], points_[2]));
  double rad = GetDistPointLine(Line(points_[0], points_[1]), center);
  return Circle(center, rad);
}
Point Triangle::centroid() const {
  Point zxc((points_[0].x + points_[1].x + points_[2].x) / 3.0,
            (points_[0].y + points_[1].y + points_[2].y) / 3.0);
  return zxc;
}
Point Triangle::orthocenter() const {
  Line first_line(points_[0], points_[1]);
  Line second_line(points_[0], points_[2]);
  Point prj_on_first = GetPrOnLine(first_line, points_[2]);
  Point prj_on_scnd = GetPrOnLine(second_line, points_[1]);
  return IntersectLines(Line(points_[2], prj_on_first),
                        Line(points_[1], prj_on_scnd));
}
Circle Triangle::ninePointsCircle() const {
  Triangle mid_triangle(GetMidOfSeqment(points_[0], points_[1]),
                        GetMidOfSeqment(points_[1], points_[2]),
                        GetMidOfSeqment(points_[2], points_[0]));
  return mid_triangle.circumscribedCircle();
}
Line Triangle::EulerLine() const {
  return Line(orthocenter(), circumscribedCircle().center());
}
