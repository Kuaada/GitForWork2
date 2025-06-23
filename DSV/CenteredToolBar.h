/**
 * @file    CenteredToolBar.h
 * @brief   Centered toolbar class, providing drawing and operation tools
 * @author  [JianZhang] ([])
 * @date    2025-01-19
 * @version 1.0.0
 * @details This file implements the toolbar functionality for the DSV project, including:
 *          - Drawing tools: lines, rectangles, ellipses, etc.
 *          - Image manipulation tools: zoom, pan, rotate
 *          - File operation tools: save, load, etc.
 *          - Measurement and annotation tools
 *          - Display of scale and resolution information
 *          - Signal and slot display controls
 *          This is one of the main UI components of the DSV project, providing
 *          users with convenient drawing and annotation tools.
 *
 * @note    This class inherits from QToolBar and is one of the main UI components of the DSV project
 * @see     MainWin, PathologyViewer, RenderElement
 */

#pragma once

#include <QToolBar>
#include <QAction>
#include <QHBoxLayout>
#include <QWidget>
#include <QWidgetAction>
#include <QLabel>
#include <QLineEdit>

 /**
  * @class  CenteredToolBar
  * @brief  Centered toolbar class, providing drawing and operation tools
  * @details This is the main toolbar class of the DSV project, providing:
  *          - Various drawing tool buttons
  *          - Display of current scale and microns per pixel values
  *          - Image manipulation and measurement tools
  *          - Tool selection and state switching
  *          - Signal and slot communication mechanisms
  *
  *          Main features include:
  *          - Drawing tools: lines, rectangles, ellipses, text, contour drawing tools
  *          - Image manipulation: zoom, pan, rotate, and other image operations
  *          - File operations: save, load, and other file management functions
  *          - Measurement tools: distance, angle, area measurement tools
  *          - Display controls: real-time display of scale and microns per pixel values
  *          - Style settings: visual effects and layout settings
  *
  * @note   This class inherits from QToolBar and provides comprehensive toolbar functionality
  * @example
  *          // Usage example
  *          CenteredToolBar* toolbar = new CenteredToolBar(parent);
  *
  *          // Set scale factor display
  *          toolbar->setFactor(1.5f);
  *
  *          // Set microns per pixel value
  *          toolbar->setMpp(0.5f);
  *
  *          // The toolbar will automatically display various tool buttons
  *          // based on user's tool selection
  * @see     MainWin, PathologyViewer, RenderElement
  */
class CenteredToolBar : public QToolBar
{
    Q_OBJECT

public:
    /**
     * @brief   Constructor
     * @details Creates the centered toolbar and initializes all tool buttons
     *
     * @param   parent Parent widget pointer
     * @note    The constructor calls setupStyle() and setupAction() to initialize the toolbar
     * @see     ~CenteredToolBar, setupStyle, setupAction
     */
    explicit CenteredToolBar(QWidget* parent);

    /**
     * @brief   Destructor
     * @details Cleans up allocated resources
     * @note    The destructor releases all toolbar button resources
     * @see     CenteredToolBar
     */
    ~CenteredToolBar();

signals:
    /**
     * @brief   Signal for microns per pixel value change
     * @details Emitted when the microns per pixel value changes
     *
     * @param   num New microns per pixel value
     * @note    This signal is used to notify other components of value changes
     */
    void TransChangedMpp(float num);

private:
    /** @brief Toolbar height */
    int m_iToolBarHeight;

    /**
     * @brief   Setup toolbar style
     * @details Sets up toolbar visual effects and layout
     * @note    This function sets toolbar colors, fonts, and layout styles
     * @see     setupAction
     */
    void setupStyle();

    /**
     * @brief   Setup toolbar actions
     * @details Creates and configures all toolbar buttons and actions
     * @note    This function creates various drawing tools, image manipulation tools, etc.
     * @see     setupStyle, createZoomDisplayAction, createScaleDisplayAction
     */
    void setupAction();

    /**
     * @brief   Create zoom display action
     * @details Creates a toolbar action for displaying zoom scale
     *
     * @param   icon Action icon
     * @param   prefix Display prefix text
     * @return  Returns QWidgetAction pointer
     * @note    This function creates an action with icon and text display
     * @see     createScaleDisplayAction
     */
    QWidgetAction* createZoomDisplayAction(const QIcon& icon, const QString& prefix);

    /**
     * @brief   Create scale display action
     * @details Creates a toolbar action for displaying microns per pixel value
     *
     * @param   prefix Display prefix text
     * @return  Returns QWidgetAction pointer
     * @note    This function creates an action with text display
     * @see     createZoomDisplayAction
     */
    QWidgetAction* createScaleDisplayAction(const QString& prefix);

    /** @brief Zoom display text edit pointer */
    QLineEdit* zoomDisplayLineEdit;

    /** @brief Scale display text edit pointer */
    QLineEdit* ScaleDisplayLineEdit;

public slots:
    /**
     * @brief   Set scale factor
     * @details Updates the toolbar scale factor display
     *
     * @param   factor New scale factor
     * @note    This slot updates the zoom display text edit
     * @see     setMpp
     */
    void setFactor(float factor);

    /**
     * @brief   Set microns per pixel value
     * @details Updates the toolbar microns per pixel display
     *
     * @param   mpp New microns per pixel value
     * @note    This slot updates the scale display text edit
     * @see     setFactor
     */
    void setMpp(float mpp);
};