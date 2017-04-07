pragma Singleton
import QtQuick 2.7

import Common 1.0

// =============================================================================

QtObject {
  property int height: 60

  property QtObject error: QtObject {
    property color color: Colors.error
    property int fontSize: 11
  }

  property QtObject loadingIndicator: QtObject {
    property int height: 20
    property int width: 20
  }
}
