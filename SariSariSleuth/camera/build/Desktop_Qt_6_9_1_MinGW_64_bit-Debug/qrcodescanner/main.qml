import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia
import com.dynamsoft.barcode

Window {
    id: window
    width: 640
    height: 480
    visible: true
    title: "QR Code Scanner"

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        VideoOutput {
            id: videoOutput
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        TextArea {
            id: resultText
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            readOnly: true
            wrapMode: TextEdit.WordWrap
        }
    }

    FrameProcessor {
        id: frameProcessor
        videoSink: videoOutput.videoSink

        onBarcodeDetected: function(text) {
            resultText.text = text
        }
    }

    Camera {
        id: camera
    }

    Component.onCompleted: {
        videoOutput.source = camera
        camera.start()
    }
}
