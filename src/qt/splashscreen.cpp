// Copyright (c) 2011-present The DRIP developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bitcoin-build-config.h> // IWYU pragma: keep

#include <qt/splashscreen.h>

#include <clientversion.h>
#include <common/system.h>
#include <interfaces/handler.h>
#include <interfaces/node.h>
#include <interfaces/wallet.h>
#include <qt/guiutil.h>
#include <qt/networkstyle.h>
#include <qt/walletmodel.h>
#include <util/translation.h>

#include <functional>

#include <QApplication>
#include <QCloseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QScreen>
#include <QTimer>
#include <QtMath>


SplashScreen::SplashScreen(const NetworkStyle* networkStyle)
    : QWidget(), m_animationAngle(0)
{
    // Set up modern dark theme colors
    m_bgColor = QColor(13, 17, 23);           // --bg-primary
    m_accentColor = QColor(0, 212, 170);       // --accent (teal)
    m_textColor = QColor(230, 237, 243);       // --text-primary
    m_subtextColor = QColor(139, 148, 158);    // --text-secondary

    float devicePixelRatio = static_cast<QGuiApplication*>(QCoreApplication::instance())->devicePixelRatio();

    // Define text to place
    QString titleText = CLIENT_NAME;
    QString versionText = QString("Version %1").arg(QString::fromStdString(FormatFullVersion()));
    QString copyrightText = QString::fromUtf8(CopyrightHolders(strprintf("\xc2\xA9 %u-%u ", 2009, COPYRIGHT_YEAR)).c_str());
    m_titleAddText = networkStyle->getTitleAddText();

    // Store for painting
    m_titleText = titleText;
    m_versionText = versionText;
    m_copyrightText = copyrightText;
    m_networkStyle = networkStyle;

    // Create a bitmap according to device pixel ratio
    QSize splashSize(520 * devicePixelRatio, 340 * devicePixelRatio);
    pixmap = QPixmap(splashSize);
    pixmap.setDevicePixelRatio(devicePixelRatio);

    // Initial render
    renderSplash();

    // Set window properties
    setWindowTitle(titleText + " " + m_titleAddText);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, false);

    // Resize window and move to center of desktop
    QRect r(QPoint(), QSize(pixmap.size().width() / devicePixelRatio, pixmap.size().height() / devicePixelRatio));
    resize(r.size());
    setFixedSize(r.size());
    move(QGuiApplication::primaryScreen()->geometry().center() - r.center());

    installEventFilter(this);
    GUIUtil::handleCloseWindowShortcut(this);

    // Start animation timer for the loading spinner
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, &SplashScreen::updateAnimation);
    m_animationTimer->start(30); // ~33 FPS for smooth animation
}

void SplashScreen::renderSplash()
{
    float devicePixelRatio = pixmap.devicePixelRatio();
    int width = pixmap.width() / devicePixelRatio;
    int height = pixmap.height() / devicePixelRatio;

    QPainter pixPaint(&pixmap);
    pixPaint.setRenderHint(QPainter::Antialiasing, true);
    pixPaint.setRenderHint(QPainter::TextAntialiasing, true);
    pixPaint.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // Draw dark background with subtle gradient
    QLinearGradient bgGradient(0, 0, width, height);
    bgGradient.setColorAt(0, m_bgColor);
    bgGradient.setColorAt(1, QColor(22, 27, 34)); // --bg-secondary
    pixPaint.fillRect(pixmap.rect(), bgGradient);

    // Draw subtle accent glow in top-left corner
    QRadialGradient glowGradient(QPointF(0, 0), 300);
    glowGradient.setColorAt(0, QColor(0, 212, 170, 30));
    glowGradient.setColorAt(1, QColor(0, 212, 170, 0));
    pixPaint.fillRect(pixmap.rect(), glowGradient);

    // Draw the DRIP icon centered
    const QSize iconSize(180, 180);
    QPixmap icon(m_networkStyle->getAppIcon().pixmap(QSize(1024, 1024)));
    QRect iconRect(QPoint((width - iconSize.width()) / 2, 40), iconSize);
    pixPaint.drawPixmap(iconRect, icon);

    // Draw animated ring around the icon (loading indicator)
    drawLoadingRing(pixPaint, QPointF(width / 2, 40 + iconSize.height() / 2), 105);

    // Draw title text
    QFont titleFont = QApplication::font();
    titleFont.setPixelSize(32);
    titleFont.setWeight(QFont::Bold);
    titleFont.setLetterSpacing(QFont::AbsoluteSpacing, 2);
    pixPaint.setFont(titleFont);
    pixPaint.setPen(m_textColor);
    
    QFontMetrics titleFm(titleFont);
    int titleWidth = GUIUtil::TextWidth(titleFm, m_titleText);
    pixPaint.drawText((width - titleWidth) / 2, 250, m_titleText);

    // Draw version text
    QFont versionFont = QApplication::font();
    versionFont.setPixelSize(13);
    pixPaint.setFont(versionFont);
    pixPaint.setPen(m_subtextColor);
    
    QFontMetrics versionFm(versionFont);
    int versionWidth = GUIUtil::TextWidth(versionFm, m_versionText);
    pixPaint.drawText((width - versionWidth) / 2, 272, m_versionText);

    // Draw copyright text
    QFont copyrightFont = QApplication::font();
    copyrightFont.setPixelSize(10);
    pixPaint.setFont(copyrightFont);
    pixPaint.setPen(QColor(m_subtextColor.red(), m_subtextColor.green(), m_subtextColor.blue(), 150));
    
    QFontMetrics copyrightFm(copyrightFont);
    int copyrightWidth = GUIUtil::TextWidth(copyrightFm, m_copyrightText);
    pixPaint.drawText((width - copyrightWidth) / 2, height - 15, m_copyrightText);

    // Draw network badge if not mainnet
    if (!m_titleAddText.isEmpty()) {
        QFont badgeFont = QApplication::font();
        badgeFont.setPixelSize(11);
        badgeFont.setWeight(QFont::Bold);
        pixPaint.setFont(badgeFont);
        
        QFontMetrics badgeFm(badgeFont);
        int badgeTextWidth = GUIUtil::TextWidth(badgeFm, m_titleAddText);
        int badgePadding = 8;
        int badgeHeight = 22;
        int badgeWidth = badgeTextWidth + badgePadding * 2;
        
        QRect badgeRect(width - badgeWidth - 15, 15, badgeWidth, badgeHeight);
        
        // Draw badge background
        pixPaint.setPen(Qt::NoPen);
        pixPaint.setBrush(QColor(0, 212, 170, 40));
        pixPaint.drawRoundedRect(badgeRect, 4, 4);
        
        // Draw badge border
        pixPaint.setPen(QPen(m_accentColor, 1));
        pixPaint.setBrush(Qt::NoBrush);
        pixPaint.drawRoundedRect(badgeRect, 4, 4);
        
        // Draw badge text
        pixPaint.setPen(m_accentColor);
        pixPaint.drawText(badgeRect, Qt::AlignCenter, m_titleAddText);
    }

    pixPaint.end();
}

void SplashScreen::drawLoadingRing(QPainter& painter, const QPointF& center, int radius)
{
    painter.save();
    
    // Draw background ring
    QPen bgPen(QColor(48, 54, 61), 3);  // --border color
    bgPen.setCapStyle(Qt::RoundCap);
    painter.setPen(bgPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(center, radius, radius);
    
    // Draw animated arc
    QPen arcPen(m_accentColor, 3);
    arcPen.setCapStyle(Qt::RoundCap);
    painter.setPen(arcPen);
    
    int startAngle = m_animationAngle * 16;  // Qt uses 1/16th of a degree
    int spanAngle = 90 * 16;  // 90 degree arc
    
    QRectF arcRect(center.x() - radius, center.y() - radius, radius * 2, radius * 2);
    painter.drawArc(arcRect, startAngle, spanAngle);
    
    painter.restore();
}

void SplashScreen::updateAnimation()
{
    m_animationAngle = (m_animationAngle + 5) % 360;
    renderSplash();
    update();
}

SplashScreen::~SplashScreen()
{
    if (m_animationTimer) {
        m_animationTimer->stop();
    }
    if (m_node) unsubscribeFromCoreSignals();
}

void SplashScreen::setNode(interfaces::Node& node)
{
    assert(!m_node);
    m_node = &node;
    subscribeToCoreSignals();
    if (m_shutdown) m_node->startShutdown();
}

void SplashScreen::shutdown()
{
    m_shutdown = true;
    if (m_node) m_node->startShutdown();
}

bool SplashScreen::eventFilter(QObject * obj, QEvent * ev) {
    if (ev->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(ev);
        if (keyEvent->key() == Qt::Key_Q) {
            shutdown();
        }
    }
    return QObject::eventFilter(obj, ev);
}

static void InitMessage(SplashScreen *splash, const std::string &message)
{
    bool invoked = QMetaObject::invokeMethod(splash, "showMessage",
        Qt::QueuedConnection,
        Q_ARG(QString, QString::fromStdString(message)),
        Q_ARG(int, Qt::AlignBottom|Qt::AlignHCenter),
        Q_ARG(QColor, QColor(139, 148, 158)));  // Use subtle text color
    assert(invoked);
}

static void ShowProgress(SplashScreen *splash, const std::string &title, int nProgress, bool resume_possible)
{
    InitMessage(splash, title + std::string("\n") +
            (resume_possible ? SplashScreen::tr("(press q to shutdown and continue later)").toStdString()
                                : SplashScreen::tr("press q to shutdown").toStdString()) +
            strprintf("\n%d", nProgress) + "%");
}

void SplashScreen::subscribeToCoreSignals()
{
    // Connect signals to client
    m_handler_init_message = m_node->handleInitMessage(std::bind(InitMessage, this, std::placeholders::_1));
    m_handler_show_progress = m_node->handleShowProgress(std::bind(ShowProgress, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    m_handler_init_wallet = m_node->handleInitWallet([this]() { handleLoadWallet(); });
}

void SplashScreen::handleLoadWallet()
{
#ifdef ENABLE_WALLET
    if (!WalletModel::isWalletEnabled()) return;
    m_handler_load_wallet = m_node->walletLoader().handleLoadWallet([this](std::unique_ptr<interfaces::Wallet> wallet) {
        m_connected_wallet_handlers.emplace_back(wallet->handleShowProgress(std::bind(ShowProgress, this, std::placeholders::_1, std::placeholders::_2, false)));
        m_connected_wallets.emplace_back(std::move(wallet));
    });
#endif
}

void SplashScreen::unsubscribeFromCoreSignals()
{
    // Disconnect signals from client
    m_handler_init_message->disconnect();
    m_handler_show_progress->disconnect();
    for (const auto& handler : m_connected_wallet_handlers) {
        handler->disconnect();
    }
    m_connected_wallet_handlers.clear();
    m_connected_wallets.clear();
}

void SplashScreen::showMessage(const QString &message, int alignment, const QColor &color)
{
    curMessage = message;
    curAlignment = alignment;
    curColor = color;
    update();
}

void SplashScreen::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.drawPixmap(0, 0, pixmap);
    
    // Draw status message with modern styling
    if (!curMessage.isEmpty()) {
        QFont msgFont = QApplication::font();
        msgFont.setPixelSize(11);
        painter.setFont(msgFont);
        painter.setPen(curColor);
        
        QRect r = rect().adjusted(20, 20, -20, -35);
        painter.drawText(r, curAlignment, curMessage);
    }
}

void SplashScreen::closeEvent(QCloseEvent *event)
{
    shutdown(); // allows an "emergency" shutdown during startup
    event->ignore();
}
