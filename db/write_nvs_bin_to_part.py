import sys
import os


idf_path = os.environ["IDF_PATH"]
parttool_dir = os.path.join(idf_path, "components", "partition_table")

sys.path.append(parttool_dir)
from parttool import *

target = ParttoolTarget("/dev/ttyUSB0")

target.write_partition(PartitionName("nvs"), "db/nvs.bin")
