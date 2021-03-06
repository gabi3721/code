#include "renderarea.h"
#include "jarvis_march.h"

#include <QPainter>
#include <QPainterPath>
#include <QColorDialog>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>

#include <ctime>
#include <cstdlib>

using namespace std;

RenderArea::RenderArea(QWidget *parent) : QWidget(parent)
{
  auto pal = palette();
  pal.setColor(QPalette::Background, Qt::black);
  setPalette(pal);
  setAutoFillBackground(true);

  selected = points.end();
  srand(static_cast<unsigned>(time(nullptr)));

  this->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(selectBgColor(const QPoint&)));
}

auto RenderArea::minimumSizeHint() const -> QSize
{
  return QSize(100, 100);
}

auto RenderArea::sizeHint() const -> QSize
{
  return QSize(600, 1000);
}

void RenderArea::numberOfPoint(int count)
{
  this->count = count;
}

void RenderArea::setPen(const QPen& pen)
{
  this->pen = pen;
  update();
}

void RenderArea::setBrush(const QBrush& brush)
{
  this->brush = brush;
  update();
}

void RenderArea::clearArea()
{
  points.clear(); poly_mode = false; update();
}

void RenderArea::findHull()
{
  if (points.size() > 0)
  {
    poly_mode = true;
    hull.compute_hull(points);
    points.swap(hull.points);
    update();
  }
}

void RenderArea::selectBgColor(const QPoint& pos)
{
  QMenu contextMenu(tr("바탕색"), this);

  QAction a1("파랑색", this);
  connect(&a1, &QAction::triggered, this, [this] { changeBgColor(Qt::blue); });
  contextMenu.addAction(&a1);

  QAction a2("흰색", this);
  connect(&a2, &QAction::triggered, this, [this] { changeBgColor(Qt::white); });
  contextMenu.addAction(&a2);

  contextMenu.exec(mapToGlobal(pos));
}

void RenderArea::changeBgColor(QColor color)
{
  auto pal = palette();
  pal.setColor(QPalette::Background, color);
  setPalette(pal);
  setAutoFillBackground(true);
}

void RenderArea::paintEvent(QPaintEvent*)
{
  try
  {
    QPainter painter(this);
    
    if (poly_mode)
      drawHull(painter);
    
    for (auto& point: points)
      drawPoint(painter, point);
  }
  catch (...)
  {
  }
}

void RenderArea::drawHull(QPainter& painter)
{
  painter.setPen(QPen(Qt::blue, 2));
  QPainterPath path;
  for (size_t i=0ul; i<hull.count; i++)
  {
    i == 0 ? path.moveTo(points[i].x, points[i].y)
           : path.lineTo(points[i].x, points[i].y);
  }

  path.closeSubpath();
  painter.drawPath(path);
}

void RenderArea::drawPoint(QPainter& painter, const Point& point)
{
  auto x = point.x;
  auto y = point.y;

  painter.setPen(QPen(QColor(255,165,0), 2));

  painter.setBrush(point.color);
  painter.drawRect(QRect(x - SZ , y - SZ, SZ * 2, SZ * 2));

  QPainterPath r;
  r.moveTo(x - SZ, y - SZ);
  r.lineTo(x - SZ, y + SZ);
  r.lineTo(x + SZ, y + SZ);
  r.lineTo(x + SZ, y - SZ);
  r.closeSubpath();

  painter.drawPath(r);
}

void RenderArea::mousePressEvent(QMouseEvent* event)
{
  if (event->button() != Qt::LeftButton)
      return;

  auto x = event->x();
  auto y = event->y();

  selected = find_if (points.begin(), points.end(),
                      [&](auto& p) { return p.located_near(x, y, SZ); });

  if (selected == points.end())
  {
    points.emplace_back(x, y, randomColor());

    for (int i=0; i<count-1; i++)
    {
      auto point = randomPoint();
      points.emplace_back(point.x, point.y, point.color);
    }
  }

  update();
}

void RenderArea::mouseReleaseEvent(QMouseEvent*)
{
  selected = points.end();
}

void RenderArea::mouseMoveEvent(QMouseEvent* event)
{
  if (selected != points.end())
  {
    auto index = static_cast<unsigned long>(distance(points.begin(), selected));
    if (index < points.size())
    {
      points[index].x = int(event->x());
      points[index].y = int(event->y());

      update();
    }
  }
}

auto RenderArea::randomPoint() -> ColorPoint<QColor>
{
  auto c = randomColor();
  auto x = rand() % (this->width()  - 20) + 10;
  auto y = rand() % (this->height() - 20) + 10;

  return ColorPoint<QColor>(x, y, c);
}

auto RenderArea::randomColor() -> QColor
{
  static std::vector<QColor> colors = {
    Qt::black,   Qt::darkGray,  Qt::lightGray,
    Qt::white,   Qt::gray,      Qt::red,
    Qt::green,   Qt::blue,      Qt::cyan,
    Qt::yellow,  Qt::magenta,
    QColor(255,165,0),
    QColor(128,0,218),
    QColor(0,128,128)
  };

  auto r = rand() % 256;
  auto g = rand() % 256;
  auto b = rand() % 256;

  return QColor(r, g, b);
}
