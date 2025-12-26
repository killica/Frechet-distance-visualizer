#pragma once

#include <QWidget>
#include "../geometry/freespace.h"

class FreeSpaceCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit FreeSpaceCanvas(QWidget* parent = nullptr);

    void setFreeSpace(const FreeSpace* fs);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    const FreeSpace* fs_ = nullptr;
    int cellSize_ = 200;   // px
};
