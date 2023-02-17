#!/usr/bin/env python
# -*- coding: utf-8 -*-

from vtkmodules.vtkIOXML import vtkXMLImageDataReader
from vtkmodules.vtkCommonCore import vtkShortArray


def get_program_parameters():
    import argparse
    description = 'Read a VTK image data file.'
    epilogue = ''''''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filename', help='vase.vti')
    args = parser.parse_args()
    return args.filename


def main():
    file_name = get_program_parameters()

    # Read the source file.
    reader = vtkXMLImageDataReader()
    reader.SetFileName(file_name)
    reader.Update()

    data = reader.GetOutput()

    pointData = data.GetPointData()
    scalars = pointData.GetArray(0)

    nT = scalars.GetNumberOfTuples()
    count = dict()
    for i in range(0, nT):
        val = scalars.GetTuple(i)[0]
        
        if val in count:
            count[val] = count[val] + 1
        else:
            count[val] = 0

    print(count)


    


if __name__ == '__main__':
    main()
