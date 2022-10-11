
import re
import os
import sys
import csv
import mmds
import logging
import traceback

source = """
#include <flavornet.h>

/*ingredients*/

const char *ingredientNames[] = {
/*ingredientNames*/
};

const float *ingredientVectors[] = {
/*ingredientVectors*/
};

int nameCount = /*nameCount*/;

int vectorSize = /*vectorSize*/;
"""

try:
    if (__name__ == "__main__"):
        print(str(sys.argv))
        if (len(sys.argv) > 2):
            content = ""
            with open(sys.argv[1], "r") as descriptor:
                content += descriptor.read()
                descriptor.close()
            content = re.sub(r"^#\s*#(\s|\w)*#(\s|\w|\.|-|,)*#", "left,right,share", content.replace("\r", ""))
            with open(sys.argv[1], "w") as descriptor:
                descriptor.write(content)
                descriptor.close()
            shares = {}
            with open(sys.argv[1], "r") as descriptor:
                reader = csv.DictReader(descriptor)
                for row in reader:
                    left = row["left"]
                    right = row["right"]
                    share = row["share"]
                    if not (left in shares):
                        shares[left] = {}
                    shares[left][left] = 0
                    if not (right in shares[left]):
                        shares[left][right] = int(share)
                    temp = left
                    left = right
                    right = temp
                    if not (left in shares):
                        shares[left] = {}
                    shares[left][left] = 0
                    if not (right in shares[left]):
                        shares[left][right] = int(share)
                descriptor.close()
            target = os.path.join(os.getcwd(), sys.argv[0]+".tsv")
            with open(target, "w") as descriptor:
                columns = []
                rows = []
                line = ""
                for key in shares:
                    column = key.strip()
                    line += column+"\t"
                    #columns.append(column)
                descriptor.write("\t"+line.strip()+"\n")
                for key in shares:
                    row = key.strip()
                    line = row+"\t"
                    rows.append(row)
                    for other in shares:
                        if (other == key):
                            line += "0.0\t"
                            continue
                        if not (other in shares[key]):
                            line += "1.0\t"
                            continue
                        line += str(1.0/float(int(shares[key][other])+1))+"\t"
                    descriptor.write(line.strip()+"\n")
                descriptor.close()
            frame = mmds.read_dm(target)
            space = mmds.Space(frame)
            active = space.active
            target += ".csv"
            with open(target, "w") as descriptor:
                descriptor.write(active.to_csv())
                descriptor.close()
            vectors = {}
            size = 0
            with open(target, "r") as descriptor:
                reader = csv.DictReader(descriptor)
                for row in reader:
                    keys = list(row.keys())
                    if (len(keys) < 2):
                        continue
                    key = keys[0]
                    keys = keys[1:]
                    size = len(keys)
                    vector = []
                    for i in range(len(keys)):
                        vector.append(float(row[keys[i]]))
                    vectors[row[key]] = vector
            strings = ""
            names = ""
            ingredients = ""
            index = 0
            for key in vectors:
                lines = ""
                for dimension in vectors[key]:
                    dimension = str(dimension)
                    if not ("." in dimension):
                        dimension += "."
                    dimension += "f, "
                    lines += dimension
                ingredients += "const float vec_"+str(index)+"[] = {\n\t"+lines+"\n};\n"
                strings += "\t\""+key+"\",\n"
                names += "\tvec_"+str(index)+",\n"
                index += 1
            with open(sys.argv[2], "w") as descriptor:
                descriptor.write(source.replace("/*ingredients*/", ingredients).replace("/*ingredientNames*/", strings).replace("/*ingredientVectors*/", names).replace("/*nameCount*/", str(index)).replace("/*vectorSize*/", str(size)))
                descriptor.close()
except:
    logging.error(traceback.format_exc())

