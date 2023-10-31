#include "DialogSessionBuilds.h"

#include "BuildData.h"
#include "SigbuildDefines.h"

#include <QDateTime>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QResizeEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QTableWidget>
#include <QVBoxLayout>

#include <QDebug>

namespace Sigbuild
{

// ==== CONSTRUCTOR / DESTRUCTOR ====

DialogSessionBuilds::DialogSessionBuilds(const QVector<BuildData *> & data,
                                         const QVector<QPixmap> & icons, QWidget * parent)
    : QDialog(parent, Qt::Dialog | Qt::WindowTitleHint| Qt::WindowCloseButtonHint)
    , mLayoutHeader(nullptr)
    , mLayoutArea(nullptr)
    , mScrollArea(nullptr)
    , mSpacerHeader(nullptr)
    , mScrollbarVisible(false)
{
    // -- properties --
    setWindowTitle(tr("Session builds"));
    setSizeGripEnabled(false);

    // -- main layout --
    QVBoxLayout * layout = new QVBoxLayout;
    setLayout(layout);

    // -- header --
    QWidget * header = new QWidget;

    layout->addWidget(header);

    QFont font;
    font.setBold(true);

    const int WIDGET_SPACING = 0;

    const int MARGIN_W = 20;

    const int STRETCH[NUM_TAB_COLUMNS] = { 30, 25, 25, 15, 5 };

    mLayoutHeader = new QHBoxLayout;
    mLayoutHeader->setContentsMargins(0, 0, 0, 5);
    mLayoutHeader->setSpacing(WIDGET_SPACING);
    header->setLayout(mLayoutHeader);

    QLabel * headerLabels[NUM_TAB_COLUMNS];

    headerLabels[COL_PROJECT] = new QLabel(tr("PROJECT"));
    headerLabels[COL_PROJECT]->setFont(font);
    headerLabels[COL_PROJECT]->setContentsMargins(0, 0, MARGIN_W, 0);
    mLayoutHeader->addWidget(headerLabels[COL_PROJECT], STRETCH[COL_PROJECT]);

    headerLabels[COL_START] = new QLabel(tr("BUILD START"));
    headerLabels[COL_START]->setFont(font);
    headerLabels[COL_START]->setContentsMargins(MARGIN_W, 0, MARGIN_W, 0);
    mLayoutHeader->addWidget(headerLabels[COL_START], STRETCH[COL_START]);

    headerLabels[COL_END] = new QLabel(tr("BUILD FINISH"));
    headerLabels[COL_END]->setFont(font);
    headerLabels[COL_END]->setContentsMargins(MARGIN_W, 0, MARGIN_W, 0);
    mLayoutHeader->addWidget(headerLabels[COL_END], STRETCH[COL_END]);

    headerLabels[COL_TIME] = new QLabel(tr("BUILD TIME"));
    headerLabels[COL_TIME]->setFont(font);
    headerLabels[COL_TIME]->setContentsMargins(MARGIN_W, 0, MARGIN_W, 0);
    mLayoutHeader->addWidget(headerLabels[COL_TIME], STRETCH[COL_TIME]);

    headerLabels[COL_RESULT] = new QLabel(tr("RESULT"));
    headerLabels[COL_RESULT]->setFont(font);
    headerLabels[COL_RESULT]->setContentsMargins(MARGIN_W, 0, 0, 0);
    mLayoutHeader->addWidget(headerLabels[COL_RESULT], STRETCH[COL_RESULT]);

    // SCROLL AREA
    mScrollArea = new QScrollArea(this);
    mScrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    mScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mScrollArea->setWidgetResizable(true);
    mScrollArea->setFrameShape(QFrame::NoFrame);

    layout->addWidget(mScrollArea);

    QWidget * scrollContent = new QWidget;
    scrollContent->setContentsMargins(0, 0, 0, 0);
    mScrollArea->setWidget(scrollContent);
    mLayoutArea = new QVBoxLayout;
    mLayoutArea->setSpacing(WIDGET_SPACING);
    mLayoutArea->setContentsMargins(0, 0, 0, 0);
    scrollContent->setLayout(mLayoutArea);

    // -- builds data --
    for(int i = 0; i < data.size(); ++i)
    {
        const BuildData * entry = data[i];

        QHBoxLayout * layoutRow = new QHBoxLayout;
        layoutRow->setSpacing(WIDGET_SPACING);
        layoutRow->setContentsMargins(0, 0, 0, 3);
        mLayoutArea->addLayout(layoutRow);

        QLabel * label = new QLabel(entry->GetProject());

        label->setContentsMargins(0, 0, MARGIN_W, 0);
        layoutRow->addWidget(label, STRETCH[COL_PROJECT]);

        // START
        label = new QLabel(QDateTime::fromMSecsSinceEpoch(entry->GetTimeStart()).toString("dd-MM-yyyy HH:mm:ss"));
        label->setContentsMargins(MARGIN_W, 0, MARGIN_W, 0);
        layoutRow->addWidget(label, STRETCH[COL_START]);

        // END
        label = new QLabel(QDateTime::fromMSecsSinceEpoch(entry->GetTimeEnd()).toString("dd-MM-yyyy HH:mm:ss"));
        label->setContentsMargins(MARGIN_W, 0, MARGIN_W, 0);
        layoutRow->addWidget(label, STRETCH[COL_END]);

        // TIME
        const qint64 diff = entry->GetTimeEnd() - entry->GetTimeStart();
        QTime buildTime(0, 0, 0, 0);
        buildTime = buildTime.addMSecs(diff);
        label = new QLabel(buildTime.toString("hh:mm:ss"));
        label->setContentsMargins(MARGIN_W, 0, MARGIN_W, 0);
        layoutRow->addWidget(label, STRETCH[COL_TIME]);

        // RESULT
        label = new QLabel;
        label->setContentsMargins(MARGIN_W, 0, 0, 0);
        label->setPixmap(icons[static_cast<int>(entry->GetState())]);
        layoutRow->addWidget(label, STRETCH[COL_RESULT]);
    }

    QScrollBar * bar = mScrollArea->verticalScrollBar();

    mSpacerHeader = new Spacer;
    mLayoutHeader->addWidget(mSpacerHeader);

    if(bar)
    {
        bar->installEventFilter(this);

        mScrollbarVisible = bar->isVisible();

        mSpacerHeader->setFixedWidth(bar->width());
        mSpacerHeader->setFixedHeight(1);

        mSpacerHeader->setVisible(mScrollbarVisible);
    }

    // -- OK BUTTON --
    QPushButton * button = new QPushButton(tr("OK"));

    button->setDefault(true);

    layout->addWidget(button, 0, Qt::AlignCenter);

    connect(button, &QPushButton::clicked, this, &QDialog::accept);
}

void DialogSessionBuilds::showEvent(QShowEvent *)
{
    UpdateSizes();
}

void DialogSessionBuilds::UpdateSizes()
{
    const int ROWS = mLayoutArea->count();

    for(int c = 0; c <NUM_TAB_COLUMNS; ++c)
    {
        int maxWmsh = mLayoutHeader->itemAt(c)->widget()->minimumSizeHint().width();

        for(int r = 0; r < ROWS; ++r)
        {
            const int DWmsh = mLayoutArea->itemAt(r)->layout()->itemAt(c)->widget()->minimumSizeHint().width();

            if(DWmsh > maxWmsh)
                maxWmsh = DWmsh;
        }

        mLayoutHeader->itemAt(c)->widget()->setMinimumWidth(maxWmsh);

        for(int r = 0; r < ROWS; ++r)
            mLayoutArea->itemAt(r)->layout()->itemAt(c)->widget()->setMinimumWidth(maxWmsh);
    }

    update();
}

bool DialogSessionBuilds::eventFilter(QObject * obj, QEvent * event)
{
    QScrollBar * bar = mScrollArea->verticalScrollBar();

    if(bar != obj)
    {
        qDebug() << "ERROR filtering wrong object...";
        return false;
    }

    bool visible = false;

    if(event->type() == QEvent::Show)
        visible = true;
    else if(event->type() != QEvent::Hide)
        return false;

    if(mScrollbarVisible != visible)
    {
        mSpacerHeader->setFixedWidth(bar->width());
        mSpacerHeader->setFixedHeight(1);

        mSpacerHeader->setVisible(visible);

        mScrollbarVisible = visible;
        update();
    }

    return false;
}

} // namespace Sigbuild
