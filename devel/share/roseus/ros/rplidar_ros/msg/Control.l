;; Auto-generated. Do not edit!


(when (boundp 'rplidar_ros::Control)
  (if (not (find-package "RPLIDAR_ROS"))
    (make-package "RPLIDAR_ROS"))
  (shadow 'Control (find-package "RPLIDAR_ROS")))
(unless (find-package "RPLIDAR_ROS::CONTROL")
  (make-package "RPLIDAR_ROS::CONTROL"))

(in-package "ROS")
;;//! \htmlinclude Control.msg.html


(defclass rplidar_ros::Control
  :super ros::object
  :slots (_velocity _steer_angle ))

(defmethod rplidar_ros::Control
  (:init
   (&key
    ((:velocity __velocity) 0.0)
    ((:steer_angle __steer_angle) 0.0)
    )
   (send-super :init)
   (setq _velocity (float __velocity))
   (setq _steer_angle (float __steer_angle))
   self)
  (:velocity
   (&optional __velocity)
   (if __velocity (setq _velocity __velocity)) _velocity)
  (:steer_angle
   (&optional __steer_angle)
   (if __steer_angle (setq _steer_angle __steer_angle)) _steer_angle)
  (:serialization-length
   ()
   (+
    ;; float32 _velocity
    4
    ;; float32 _steer_angle
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32 _velocity
       (sys::poke _velocity (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _steer_angle
       (sys::poke _steer_angle (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32 _velocity
     (setq _velocity (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _steer_angle
     (setq _steer_angle (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get rplidar_ros::Control :md5sum-) "b78d7aa85d0de2901107ab6e97fb5393")
(setf (get rplidar_ros::Control :datatype-) "rplidar_ros/Control")
(setf (get rplidar_ros::Control :definition-)
      "float32 velocity
float32 steer_angle

")



(provide :rplidar_ros/Control "b78d7aa85d0de2901107ab6e97fb5393")


