#!/usr/bin/env python3

import can
import struct
import time

# Notes:
#  Must send full velocity + torque FF or we get crazy torque value + error
#  Cannot enable/disable watchdog through CAN?
#  Have to do encoder offset calibration on every startup...

HEARTBEAT = 0x001
SET_AXIS_STATE = 0x007
GET_ENCODER_ESTIMATES = 0x009
SET_INPUT_VEL = 0x00d
CLEAR_ERRORS = 0x018

AXIS_STATE_CLOSED_LOOP_CONTROL = 8


class DiffDriveBase:

    def __init__(self, left_id=0, right_id=2):
        self.left_id = left_id
        self.right_id = right_id

        self.bus = can.interface.Bus("can0", interface="socketcan")

    def send(self, node_id, cmd_id, values=None):
        self.bus.send(can.Message(
            arbitration_id=(node_id << 5 | cmd_id),
            data=values,
            is_extended_id=False
        ))

    def send_rtr(self, node_id, cmd_id, size):
        self.bus.send(can.Message(
            arbitration_id=(node_id << 5 | cmd_id),
            is_remote_frame=True,
            dlc=size,
            is_extended_id=False
        ))

    def clear_errors(self):
        self.send(self.left_id, CLEAR_ERRORS)
        self.send(self.right_id, CLEAR_ERRORS)

    def set_closed_loop(self):
        while True:
            self.send(self.left_id,
                      SET_AXIS_STATE,
                      struct.pack('<I', AXIS_STATE_CLOSED_LOOP_CONTROL))
            for msg in self.bus:
                if msg.arbitration_id == (self.left_id << 5 | HEARTBEAT):
                    error, state, result, traj_done = struct.unpack('<IBBB', bytes(msg.data[:7]))
                    if state == AXIS_STATE_CLOSED_LOOP_CONTROL:
                        print("Set control state")
                        return

    def set_velocity(self, left, right):
        self.send(self.left_id, SET_INPUT_VEL, struct.pack('<ff', left, 0.0))
        # TODO: set right velocity

    def update_velocity(self):
        self.send_rtr(self.left_id, GET_ENCODER_ESTIMATES, 8)
        # TODO: get right velocity



if __name__ == '__main__':
    d = DiffDriveBase()
    d.clear_errors()
    d.set_closed_loop()

    try:
        while True:
            d.set_velocity(1.0, 1.0)
            d.update_velocity()

            while True:
                # Read back values
                msg = d.bus.recv(timeout=0.001)
                if msg is None:
                    break

                #for msg in bus:
                if msg.arbitration_id == (d.left_id << 5 | GET_ENCODER_ESTIMATES):
                    pos, vel = struct.unpack('<ff', bytes(msg.data))
                    print(f"pos: {pos:.3f} [turns], vel: {vel:.3f} [turns/s]")

                if msg.arbitration_id == (d.left_id << 5 | HEARTBEAT):
                    error, state, result, traj_done = struct.unpack('<IBBB', bytes(msg.data[:7]))
                    print(error, state, result, traj_done)

            time.sleep(0.1)
    except KeyboardInterrupt:
        d.bus.shutdown()
