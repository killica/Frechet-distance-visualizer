#pragma once

#include <QWidget>
#include "../geometry/freespace.h"

class FreeSpaceCanvas : public QWidget
{
    Q_OBJECT
    FreeSpace* fs_ = nullptr;
public:
    explicit FreeSpaceCanvas(QWidget* parent = nullptr);

    void setFreeSpace(FreeSpace* fs);

    void updateCellSize();
protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int cellSize_ = 50;
};
