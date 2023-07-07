import argparse
import serial
import time
import numpy as np
import matplotlib.pyplot as plt
import scipy.io as io
import struct
import seaborn as sns
from mdvUtils import utils as mdv

from collections import deque

sns.set_style("whitegrid")


class ucSerde(object):
    def __init__(self, port: str, baudrate: int) -> None:
        """ucSerde

        Initializes the serial communication with the
        microcontroller and the Zeromq communication channel
        for asynchronous message passing for live-mode plotting.

        For command-line interface usage: python ucSerde.py -h

        Args:
            port (str): serial interface, '/dev/ttyACMx' for Unix-like systems,
            'COMx' for Windows systems

            baudrate (int): communication speed over the serial channel,
            [bits/s]
        """

        self.port = port
        self.baudrate = baudrate

        self.serialComm = serial.Serial(
            port=port, baudrate=baudrate, timeout=None
        )

        self._fs = 0

    def daq_config(
        self,
        fclk: int,
        psc: int,
        arr: int,
        scount: int,
        sfilter: int = 0,
        sdecim: int = 1,
    ):
        """Configures the command to be sent to the microcontroller.
        Args:
            fclk (int): FCLK [Hz], (system || timer) clock frequency
            psc (int): TIM2 Prescaler
            arr (int): AutoReloadRegister (CounterPeriod)
            scount (int): Requested number of samples,\
            sfilter (int): Toggle filter on/off. Defaults to 0.
            sdecim (int): Decimate each n-th sample. Defaults to 1.
        """

        self._fs = (fclk / (psc + 1)) / (arr + 1)
        self._ts = 1 / self._fs

        scount = scount // sdecim

        self.scount = scount
        self.sfilter = sfilter
        self.sdecim = sdecim

        self.command = "ANA {} {} {} {} {}".format(
            psc, arr, scount, sfilter, sdecim
        )

        print("Sampling frequency [Hz]: {}".format(self._fs))

        print(self.command.encode("ascii", "ignore"))

    def send_command(self):
        self.serialComm.reset_output_buffer()
        self.serialComm.write(self.command.encode())
        self.serialComm.reset_input_buffer()

    def receive_data(self):
        """Transmits the command to the microcontroller, initializing the
        periphery and starting the analog-digital converter. The converted
        signals are transmitted over UART.
        """
        print("Standby")
        tstart = time.time()
        print("Receiving data")
        self.serialComm.reset_input_buffer()
        data = self.serialComm.read_until(b"/n", size=self.scount * 2)
        self.serialComm.reset_input_buffer()
        self.serialComm.close()
        tend = time.time()
        print("Time elapsed: {}".format(tend - tstart))

        if self.sfilter == 0:
            it = struct.iter_unpack("<H", data)
            data = np.array(list(it))
        else:
            # TODO
            pass
        return data

    @property
    def fs(self):
        return self._fs

    @property
    def ts(self):
        return self._ts


def main():
    """Command-line interface for ucSerde, additionally
    plots the acquired data for testing purposes.
    """

    parser = argparse.ArgumentParser(
        description="ucSerde command line utility"
    )
    parser.add_argument(
        "port",
        action="store",
        default="/dev/ttyACM0",
        type=str,
        help="serial interface, '/dev/ttyACMx' for\
                            Unix-like systems, 'COMx' for Windows systems",
    )
    parser.add_argument(
        "baud",
        action="store",
        default=115200,
        type=int,
        help="Baudrate [uint32]",
    )
    parser.add_argument(
        "fclk",
        action="store",
        default=72000000,
        type=int,
        help="FCLK [Hz] [int]",
    )
    parser.add_argument(
        "psc",
        action="store",
        default=32 - 1,
        type=int,
        help="TIM2 Prescaler [int]",
    )
    parser.add_argument(
        "arr",
        action="store",
        default=1000 - 1,
        type=int,
        help="Counter Period (AutoReloadRegister) [int]",
    )
    parser.add_argument(
        "N",
        action="store",
        default=1024,
        type=int,
        help="Requested number of samples [uint32]",
    )
    parser.add_argument(
        "toggle_filter",
        action="store",
        default=0,
        type=int,
        help="Toggle filter on/off [bool]",
    )
    parser.add_argument(
        "dec",
        action="store",
        default=0,
        type=int,
        help="Decimate each n-th sample [uint8]",
    )
    parser.add_argument(
        "filename",
        action="store",
        default="Hz",
        type=str,
        help="Decimate each n-th sample [uint8]",
    )
    args = parser.parse_args()

    serde = ucSerde(args.port, args.baud)

    serde.daq_config(
        fclk=args.fclk,
        psc=args.psc,
        arr=args.arr,
        scount=args.N,
        sfilter=args.toggle_filter,
        sdecim=args.dec,
    )

    serde.send_command()
    data = serde.receive_data()

    k = np.arange(0, len(data), 1)

    # print(data)
    # data_c2v = mdv.code2volt(data)
    # n = len(data_c2v)

    log_flag = False

    plt.figure(1)
    plt.step(k, data)
    plt.show()

    # if data is not None:
    #     spect = mdv.spectrum(data_c2v, None,
    #                          fs=serde.fs // serde.sdecim,
    #                          log_flag=log_flag,
    #                          one_sided=True)

    #     plt.figure(1)
    #     plt.subplot(211)
    #     plt.plot(spect.f, spect.betrag)
    #     plt.xlabel('F [Hz]')
    #     plt.ylabel('Amplitude [dB]')
    #     plt.minorticks_on()
    #     plt.grid(visible=True)
    #     plt.subplot(212)
    #     plt.step(spect.t, spect.xw)
    #     plt.xlabel('Time [s]')
    #     plt.ylabel('Amplitude [V]')
    #     plt.minorticks_on()
    #     plt.grid(visible=True)
    #     plt.show()

    # io.savemat('{}.mat'.format(args.filename), {args.filename: data})

    if args.filename != "x":
        np.savetxt(args.filename + ".csv", data, delimiter=",")
        np.save(args.filename + ".npy", data)


if __name__ == "__main__":
    main()
