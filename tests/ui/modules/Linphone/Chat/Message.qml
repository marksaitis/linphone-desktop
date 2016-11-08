import QtQuick 2.7

import Linphone.Styles 1.0
import Utils 1.0

// ===================================================================

Item {
  id: container

  property alias backgroundColor: rectangle.color
  property alias color: text.color
  property alias fontSize: text.font.pointSize
  default property alias _content: content.data

  // -----------------------------------------------------------------

  implicitHeight: text.contentHeight + text.padding * 2

  Rectangle {
    id: rectangle

    height: parent.height
    radius: ChatStyle.entry.message.radius
    width: (
      text.contentWidth < parent.width
        ? text.contentWidth
        : parent.width
    ) + text.padding * 2
  }

  Text {
    id: text

    anchors {
      left: container.left
      right: container.right
    }
    padding: ChatStyle.entry.message.padding
    text: Utils.encodeUrisToQmlFormat($content)
    wrapMode: Text.Wrap

    // Little fix. Text may disappear with scrolling.
    renderType: Text.NativeRendering

    // See http://doc.qt.io/qt-5/qml-qtquick-text.html#textFormat-prop
    // and http://doc.qt.io/qt-5/richtext-html-subset.html
    textFormat: Text.StyledText

    onLinkActivated: Qt.openUrlExternally(link)

    MouseArea {
      anchors.fill: parent
      acceptedButtons: Qt.NoButton
      cursorShape: parent.hoveredLink
        ? Qt.PointingHandCursor
        : Qt.ArrowCursor
    }
  }

  Item {
    id: content

    anchors {
      left: rectangle.right
      leftMargin: ChatStyle.entry.message.extraContent.leftMargin
    }
  }
}
