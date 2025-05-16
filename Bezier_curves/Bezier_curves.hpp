#include <Eigen/Dense>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <memory>
#include <set>

class BezierCurves {
public:
  // Инициализация класса
  BezierCurves();
  // Функция запуска работы
  void work();
private:
  // Функция инициализации файлов
  void init_files();
  // Функция обработки нажатия клавиши клавиатуры
  void update_KeyPressed(std::optional<sf::Event> event);
  // Функция обработки отпускания клавиши клавиатуры
  void update_KeyReleased(std::optional<sf::Event> event);
  // Функция обработки движения мыши 
  void update_MouseMoved(std::optional<sf::Event> event);
  // Функция обработки нажатия кнопки мыши
  void update_MouseButtonPressed(std::optional<sf::Event> event);
  // Общая функция обработки событий
  void check_events();

  // Отрисовка кривой Безье
  void draw_bezier_curves(std::vector<Eigen::Vector2d> points_,
    sf::Color c);
  // Отрисовка точек, задающих кривую Безье
  void draw_points();
  // Отрисовка панели инструментов
  void draw_title();
  // Отрисовка мышки
  void draw_mouse();
  // Отрисовка линий
  void draw_line(sf::Vector2f a, sf::Vector2f b);
  // Отрисовка текста на панели инструментов
  void draw_text();
  // Отрисовка точек, делящий отрезок в отношении t
  void draw_recursive_points_on_time(double t,
    std::vector<Eigen::Vector2d>& points_,
    std::vector<std::pair<Eigen::Vector2d, 
    Eigen::Vector2d>>& dots);
  void draw_points_on_time(double t);
  // Общая функция отрисовки
  void draw();

  // Бинарное возведение a в степень b
  double bin_pow(double a, long long b);
  // Факториал до n от start(по умолщанию 1)
  double fact(int n, int start);
  // C из n по k
  double C(int n, int k);

  // Поиск точки, на позиции pos. Если такой нет, то -1
  int point_id(Eigen::Vector2d pos);
  // Если pos в рамках rect, то возвращается pos, иначе
  // точка на рамке rect.
  Eigen::Vector2d norm(const Eigen::Vector2d& pos);
  // Возвращение числа миллисекунд. 
  long long get_mc();

  // Массив точек
  std::vector<Eigen::Vector2d> points;
  // Позиция мыши
  Eigen::Vector2d mouse;
  // Рамка в которой происходит отрисовка кривой
  std::pair<Eigen::Vector2d, Eigen::Vector2d> rect = 
  { Eigen::Vector2d(0, 100),
  Eigen::Vector2d(800, 600) };

  // Радиус точки
  double radius_point = 10;
  // Если точка зафиксированна, то номер точки, иначе -1
  int fixed_point = -1;

  // Окно отрисовки
  sf::RenderWindow window;
  
  // модификация режима программы
  std::string mode = "";

  // Текстуры мыши:
  // рука
  sf::Texture grab_mouse;
  // стандартная мышь
  sf::Texture default_mouse;
  // Текстуры:
  // кнопка добавления точки
  sf::Texture add_point;
  // кнопка удаления точки
  sf::Texture del_point;
  // кнопка выхода
  sf::Texture logout;

  // Шрифт
  sf::Font font;
};