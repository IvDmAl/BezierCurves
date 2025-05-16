#include "Bezier_curves/Bezier_curves.hpp"
#include <SFML/Graphics.hpp>
#include <set>
#include <iostream>
#include <chrono>

Eigen::Vector2d to_eigen (const sf::Vector2u& rhs) {
  return Eigen::Vector2d(rhs.x, rhs.y);
}
Eigen::Vector2d to_eigen (const sf::Vector2i& rhs) {
  return Eigen::Vector2d(rhs.x, rhs.y);
}
sf::Vector2u to_sf(const Eigen::Vector2d& rhs) {
  return sf::Vector2u(rhs.x(), rhs.y());
}

long long BezierCurves::get_mc()
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  ).count();
}

void BezierCurves::init_files()
{
  if (
    !default_mouse.loadFromFile("../images/default.png") ||
    !grab_mouse.loadFromFile("../images/grab.png")       ||
    !add_point.loadFromFile("../images/add_point.png")   ||
    !del_point.loadFromFile("../images/del_point.png")   ||
    !logout.loadFromFile("../images/logout.png")         ||
    !font.openFromFile("../fonts/times.ttf"))
    throw std::runtime_error("error load");
}

BezierCurves::BezierCurves()
{
  init_files();
  sf::Vector2u size = to_sf(rect.second);
  window.create(sf::VideoMode(size), "BezierCurves", 
    sf::Style::Titlebar);
  window.setMinimumSize(size);
  window.setMaximumSize(size);
  window.setMouseCursorVisible(false);
}

Eigen::Vector2d BezierCurves::norm(const Eigen::Vector2d& pos)
{
  return Eigen::Vector2d(
  std::min(std::max(rect.first.x() + radius_point * 1.5, pos.x()),
    rect.second.x() - radius_point * 1.5),
  std::min(std::max(rect.first.y() + radius_point * 1.5, pos.y()),
    rect.second.y() - radius_point * 1.5));
}

int BezierCurves::point_id(Eigen::Vector2d pos)
{
  for (int i = 0; i < points.size(); ++i)
  {
    if ((pos - points[i]).norm() <= radius_point)
      return i;
  }
  return -1;
}

void BezierCurves::update_KeyPressed(
  std::optional<sf::Event> event)
{
  if (event->getIf<sf::Event::KeyPressed>()->code ==
    sf::Keyboard::Key::Escape)
  {
    window.close();
  }
  if (event->getIf<sf::Event::KeyPressed>()->shift)
  {
    mode = "shift";
  }
}

void BezierCurves::update_KeyReleased(
  std::optional<sf::Event> event)
{
  if (mode == "shift")
    fixed_point = -1;
  mode = "";
}

void BezierCurves::update_MouseMoved(
  std::optional<sf::Event> event)
{
  if (mode == "shift" && fixed_point != -1)
  {
    points[fixed_point] = norm(to_eigen(
      event->getIf<sf::Event::MouseMoved>()->position));
  }
}

void BezierCurves::update_MouseButtonPressed(
  std::optional<sf::Event> event)
{
  mouse = to_eigen(event->getIf<sf::Event::MouseButtonPressed>()
    ->position);

  if (mouse.y() < 100)
  {
    if (mouse.x() <= 100)
    {
      points.push_back((rect.first + rect.second) / 2);
    }
    else if (mouse.x() <= 200)
    {
      if (fixed_point != -1)
      {
        std::vector<Eigen::Vector2d> new_points;
        for (int i = 0; i < points.size(); ++i)
          if (i != fixed_point)
            new_points.push_back(points[i]);
        std::swap(points, new_points);
      }
    }
    else if (mouse.x() >= rect.second.x() - 100)
      window.close();
  }

  int point_id_ = point_id(mouse);
  if (mode == "shift")
  {
    if (mouse.y() >= 100)
    {
      if (point_id_ != -1)
      {
        points[point_id_] = mouse;
        fixed_point = point_id_;
      }
    }
  }
  else if (mode == "")
  {
    if (fixed_point == -1)
      fixed_point = point_id_;
    else
      fixed_point = -1;
  }
}

void BezierCurves::check_events()
{
  std::optional<sf::Event> event;
  while (event = window.pollEvent())
  {
    if (event->is<sf::Event::Closed>())
      window.close();

    if (event->is<sf::Event::KeyPressed>())
      update_KeyPressed(event);
    if (event->is<sf::Event::KeyReleased>())
      update_KeyReleased(event);

    if (event->is<sf::Event::MouseMoved>())
      update_MouseMoved(event);
    if (event->is<sf::Event::MouseButtonPressed>())
      update_MouseButtonPressed(event);
    if (event->is<sf::Event::MouseButtonReleased>())
    {
      if (mode == "shift")
        fixed_point = -1;
    }
  }
}

void BezierCurves::draw_bezier_curves(
  std::vector<Eigen::Vector2d> points_, sf::Color c)
{
  const double step = 0.001;
  if (points_.size() >= 2)
  {
    std::vector<sf::Vertex> line;
    for (long double t = 0; t <= 1; t += step)
    {
      Eigen::Vector2d point(0, 0);
      for (int i = 0; i < points_.size(); ++i)
      {
        double c = C(points_.size() - 1, i);
        double t1 = bin_pow(t, i);
        double t2 = bin_pow(1 - t, points_.size() - i - 1);
        point += c * t1 * t2 * points_[i];
      }

      sf::Vertex a;
      a.position = sf::Vector2f(point.x(), point.y());
      a.color = c;
      line.push_back(a);
    }
    window.draw(line.data(), line.size(), sf::PrimitiveType::LineStrip);
  }
}

void BezierCurves::draw_points()
{
  for (int i = 0; i < points.size(); ++i)
  {
    if (fixed_point == i ||
      point_id(to_eigen(sf::Mouse::getPosition(window))) == i)
    {
      sf::CircleShape cs_;
      cs_.setPosition(sf::Vector2f(points[i].x(), points[i].y()));
      cs_.setFillColor(sf::Color::Red);
      cs_.setRadius(radius_point * 1.5);
      cs_.setOrigin(sf::Vector2f(radius_point * 1.5, radius_point * 1.5));
      window.draw(cs_);
    }
    sf::CircleShape cs;
    cs.setPosition(sf::Vector2f(points[i].x(), points[i].y()));
    if (i == 0)
      cs.setFillColor(sf::Color(128, 255, 128));
    else if (i == points.size() - 1)
      cs.setFillColor(sf::Color(255, 128, 128));
    else
      cs.setFillColor(sf::Color(128, 128, 128));
    cs.setRadius(radius_point);
    cs.setOrigin(sf::Vector2f(radius_point, radius_point));
    window.draw(cs);

    sf::Text text(font, "point " + std::to_string(i + 1), 15);
    text.setPosition(sf::Vector2f(points[i].x(), points[i].y()));
    text.setFillColor(sf::Color::Black);
    window.draw(text);
  }
}

void BezierCurves::draw_line(sf::Vector2f a, sf::Vector2f b)
{
  std::vector<sf::Vertex> line(2);
  line[0].position = a;
  line[0].color = sf::Color::Black;
  line[1].position = b;
  line[1].color = sf::Color::Black;
  window.draw(line.data(), line.size(), 
    sf::PrimitiveType::LineStrip);
}

void BezierCurves::draw_text()
{
  sf::Text text(font, "Bezier Curve");
  text.setPosition(sf::Vector2f(
    200 + (rect.second.x() - 300) / 2, 15));
  text.setOrigin(text.getLocalBounds().getCenter());
  text.setFillColor(sf::Color::Black);
  window.draw(text);

  sf::Text text2(font,
    std::string("The aim of this course work was") +
    std::string(" to construct and display a Bezier curve.\n") +
    std::string("To add/remove points, press the corresponding buttons.\n") +
    std::string("To move the dots, hold down shift."), 15);
  text2.setPosition(sf::Vector2f(
    210, 30));
  text2.setFillColor(sf::Color::Black);
  window.draw(text2);
}

void BezierCurves::draw_title() {
  sf::RectangleShape Rs;
  Rs.setFillColor(sf::Color::White);
  Rs.setSize(sf::Vector2f(rect.second.x(), 100));
  window.draw(Rs);

  sf::Sprite sprite_add_point(add_point);
  sprite_add_point.setPosition(sf::Vector2f(0, 0));
  window.draw(sprite_add_point);
  sf::Sprite sprite_del_point(del_point);
  sprite_del_point.setPosition(sf::Vector2f(100, 0));
  window.draw(sprite_del_point);
  sf::Sprite sprite_logout(logout);
  sprite_logout.setPosition(sf::Vector2f(rect.second.x() - 75, 25));
  window.draw(sprite_logout);

  draw_line(sf::Vector2f(              0, 100),
            sf::Vector2f(rect.second.x(), 100));
  
  draw_line(sf::Vector2f(100, 0), 
            sf::Vector2f(100, 100));
  
  draw_line(sf::Vector2f(200, 0), 
            sf::Vector2f(200, 100));

  draw_line(sf::Vector2f(rect.second.x() - 100, 0),
            sf::Vector2f(rect.second.x() - 100, 100));
  
  draw_text();
}

void BezierCurves::draw_mouse()
{
  sf::Sprite sprite(default_mouse);
  if (mode == "shift")
    sprite.setTexture(grab_mouse);
  sprite.setPosition(static_cast<sf::Vector2f>(
    sf::Mouse::getPosition(window)));
  window.draw(sprite);
}

void BezierCurves::draw_recursive_points_on_time(double t,
  std::vector<Eigen::Vector2d>& points_,
  std::vector<std::pair<Eigen::Vector2d,
  Eigen::Vector2d>>& dots)
{
  for (int i = 0; i < points_.size() - 1; ++i)
  {
    draw_line(sf::Vector2f(to_sf(points_[i])),
      sf::Vector2f(to_sf(points_[i + 1])));
  }
  std::vector<Eigen::Vector2d> new_points;
  for (int i = 0; i < points_.size() - 1; ++i)
  {
    new_points.push_back(points_[i] +
      t * (points_[i + 1] - points_[i]));
  }
  if (new_points.size() == 1)
  {
    sf::CircleShape cs_;
    cs_.setPosition(sf::Vector2f(to_sf(new_points.front())));
    cs_.setFillColor(sf::Color(0, 0, 0));
    cs_.setRadius(radius_point * 0.5);
    cs_.setOrigin(sf::Vector2f(radius_point * 0.5,
      radius_point * 0.5));
    window.draw(cs_);

    dots.push_back({ new_points.front(), new_points.back() });

    std::vector<Eigen::Vector2d> first_part, last_part;
    for (int i = 0; i < dots.size(); ++i)
    {
      first_part.push_back(dots[i].first);
      last_part.push_back(dots[dots.size() - 1 - i].second);
    }

    draw_bezier_curves(first_part, sf::Color::Green);
    draw_bezier_curves(last_part, sf::Color::Red);
  }
  else
  {
    dots.push_back({ new_points.front(), new_points.back() });
    draw_recursive_points_on_time(t, new_points, dots);
  }
}

void BezierCurves::draw_points_on_time(double t)
{
  std::vector<std::pair<Eigen::Vector2d,
    Eigen::Vector2d>> dots = { {points.front(), points.back()}};
  draw_recursive_points_on_time(t, points, dots);

  sf::Text text(font, 
    std::string("point separation coefficient: ") +
    std::to_string(t).substr(0, 5), 20);
  text.setPosition(sf::Vector2f(0, 100));
  text.setFillColor(sf::Color::Black);
  window.draw(text);
}

void BezierCurves::draw()
{
  window.clear(sf::Color::White);
  draw_title();
  draw_bezier_curves(points, sf::Color::Black);
  if (points.size() >= 2)
    draw_points_on_time(get_mc() % 30000 / 30000.0);
  draw_points();
  
  draw_mouse();
  
  window.display();
}

double BezierCurves::bin_pow(double a, long long b)
{
  double ans = 1;
  for (long long i = 32; i >= 0; --i)
  {
    ans = ans * ans;
    if (b & (1ull << i))
      ans = ans * a;
  }
  return ans;
}
double BezierCurves::fact(int n, int start = 1)
{
  double ans = start;
  for (int i = start + 1; i <= n; ++i)
    ans *= i;
  return ans;
}
double BezierCurves::C(int n, int k)
{
  return fact(n) / fact(n - k) / fact(k);
}

void BezierCurves::work()
{
  while (window.isOpen())
  {
    check_events();
    draw();
  }
}