#include "keybindingpage.hpp"

#include <cassert>

#include <QComboBox>
#include <QGridLayout>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include "../../model/prefs/setting.hpp"
#include "../../model/prefs/category.hpp"

namespace CSVPrefs
{
    KeyBindingPage::KeyBindingPage(CSMPrefs::Category& category, QWidget* parent)
        : PageBase(category, parent)
        , mTopWidget(0)
        , mStackedLayout(0)
        , mPageLayout(0)
        , mPageSelector(0)
    {
        init();
    }

    void KeyBindingPage::addSetting(CSMPrefs::Setting *setting)
    {
        std::pair<QWidget*, QWidget*> widgets = setting->makeWidgets (this);

        if (widgets.first)
        {
            // Label, Option widgets
            assert(mPageLayout);

            int next = mPageLayout->rowCount();
            mPageLayout->addWidget(widgets.first, next, 0);
            mPageLayout->addWidget(widgets.second, next, 1);
        }
        else if (widgets.second)
        {
            // Wide single widget
            assert(mPageLayout);

            int next = mPageLayout->rowCount();
            mPageLayout->addWidget(widgets.second, next, 0, 1, 2);
        }
        else
        {
            if (setting->getLabel().empty())
            {
                // Insert empty space
                assert(mPageLayout);

                int next = mPageLayout->rowCount();
                mPageLayout->addWidget(new QWidget(), next, 0);
            }
            else
            {
                // Create new page
                QWidget* pageWidget = new QWidget();
                mPageLayout = new QGridLayout(pageWidget);
                mPageLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

                mStackedLayout->addWidget(pageWidget);

                mPageSelector->addItem(QString::fromUtf8(setting->getLabel().c_str()));
            }
        }
    }

    void KeyBindingPage::refresh()
    {
        delete mTopWidget;

        // reinitialize
        init();
    }

    void KeyBindingPage::init()
    {
        // Need one widget for scroll area
        mTopWidget = new QWidget();
        QVBoxLayout* topLayout = new QVBoxLayout(mTopWidget);

        QWidget* resetAll = new QPushButton("Reset all to default", this);
        connect(resetAll, SIGNAL(clicked()), this, SLOT(resetCategory()));
        topLayout->addWidget(resetAll);

        // Allows switching between "pages"
        QWidget* stackedWidget = new QWidget();
        mStackedLayout = new QStackedLayout(stackedWidget);

        mPageSelector = new QComboBox();
        connect(mPageSelector, SIGNAL(currentIndexChanged(int)), mStackedLayout, SLOT(setCurrentIndex(int)));

        topLayout->addWidget(mPageSelector);
        topLayout->addWidget(stackedWidget);
        topLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

        // Add each option
        for (CSMPrefs::Category::Iterator iter = getCategory().begin(); iter!=getCategory().end(); ++iter)
            addSetting (*iter);

        setWidgetResizable(true);
        setWidget(mTopWidget);
    }
}
