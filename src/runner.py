#!/usr/bin/python3
"""
Script for generating the data set (128b, 256b, 1kB, 1MB, 100MB, 1GB).

Context : Projet BCS - Master 2 SSI - Istic (Univ. Rennes1)
Authors : Andriamilanto Tompoariniaina and Genevey-metat Christophe

This script also executes the time measurement into 4 contexts
  => Sequential encryption
  => Sequential decryption
  => Parallel encryption
  => Parallel decryption
"""

import os
import time
import subprocess
from collections import OrderedDict
import pygal


B_NUMBER = 1024
B_SIZE = 16
SIXTEEN_B = None

DATASET_DIR = './dataset/'
REPORT_DIR = './report/'
DATASET_EXTENSION = '.i'
CIPHERED_EXTENSION = '.encrypted'
UNCIPHERED_EXTENSION = '.decrypted'

EXEC_NAME = './myAE.exe'

FILESIZES = OrderedDict([
    ('128b', 16),
    ('256b', 32),
    ('1kB', 1000),
    ('1MB', 1000000),
    ('100MB', 100000000),
    ('1GB', 1000000000)
])


def generate_file(name, size):
    """Generate an input file containing random bits."""
    print('=> Generating %s file' % name)
    with open(DATASET_DIR+name+DATASET_EXTENSION, 'wb+') as fout:
        fout.write(os.urandom(size))


def generate_dataset():
    """Generate dataset files."""
    print(" ### Generating the dataset files ### ")
    print('/!\\ This function can take a lot of time /!\\')

    # For every filesize, generate the file
    for key, value in FILESIZES.items():
        generate_file(key, value)


def cipher_execution(op, input, output, password):
    """Launch the encryption and measure the time it took."""
    command = [
        EXEC_NAME,
        op,
        input,
        '-o',
        output,
        '-k',
        password
    ]
    start_time = time.time()
    subprocess.call(command, 1)
    end_time = time.time() - start_time
    print("%s took %f seconds" % (input, end_time))
    return end_time


def generate_encryption_statistics():
    """Generate the figure of encryption time given the input size."""
    print("\nGeneration of the encryption statistics:")

    # Password
    password = 'password'

    # The table of the results
    results = []

    # For every filesize, generate the file
    for key in FILESIZES:
        results.append(
            cipher_execution(
                '-c',
                DATASET_DIR+key+DATASET_EXTENSION,
                DATASET_DIR+key+CIPHERED_EXTENSION,
                password
            )
        )

    line_chart = pygal.Line()
    line_chart.title = 'Execution time of encryption in sequential mode'
    line_chart.x_title = 'Size of input file'
    line_chart.x_labels = FILESIZES
    line_chart.y_title = 'Execution time in seconds'
    line_chart.add('Time', results)
    line_chart.render_to_png(REPORT_DIR+'encryption_sequential.png')


def generate_decryption_statistics():
    """Generate the figure of decryption time given the input size."""
    print("\nGeneration of the decryption statistics:")

    # Password
    password = 'password'

    # The table of the results
    results = []

    # For every filesize, generate the file
    for key in FILESIZES:
        results.append(
            cipher_execution(
                '-d',
                DATASET_DIR+key+CIPHERED_EXTENSION,
                DATASET_DIR+key+UNCIPHERED_EXTENSION,
                password
            )
        )

    line_chart = pygal.Line()
    line_chart.title = 'Execution time of decryption in sequential mode'
    line_chart.x_title = 'Size of input file'
    line_chart.x_labels = FILESIZES
    line_chart.y_title = 'Execution time in seconds'
    line_chart.add('Time', results)
    line_chart.render_to_png(REPORT_DIR+'decryption_sequential.png')


# Main function to be launched when this script is called
if __name__ == '__main__':

    # Generation of the dataset
    gen = input("Do you want to generate dataset? [y/n] ")
    if gen == 'y':
        generate_dataset()

    # Process statistics on it
    generate_encryption_statistics()
    generate_decryption_statistics()
