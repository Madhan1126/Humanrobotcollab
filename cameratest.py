import cv2

video_capture = cv2.VideoCapture(1)

while True:
    re,img = video_capture.read()
    cv2.imshow("Image",cv2.resize(img, (800,600)))
    if cv2.waitKey(1) & 0xFF == ord('q'):
        video_capture.release()
        break
    
video_capture.release()