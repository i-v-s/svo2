#from torch_svo.detect import Frame, FastDetectorCpuGrid, FastDetectorGpu
from torch_svo.init import KltHomographyInit


def main():
    #detector = (
    #    FastDetectorGpu if use_gpu else FastDetectorCpuGrid
    #)(w, h, 64, 64, 0, 1, 0, 0, 10.0, 10, pyvilib.SUM_OF_ABS_DIFF_ON_ARC)

    init = KltHomographyInit()
    print('Completed')


if __name__ == '__main__':
    main()
