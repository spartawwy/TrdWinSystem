# coding=utf-8

import os

#DATA_FILE_PATH = 'C:\\StockHisdata\\SCL8\\201901'
DATA_FILE_PATH = 'F:/StockHisdata/SCL8'
def main():
    CODE = "sc主力连续"
    mon_beg = 201901
    while mon_beg <= 201912: 
        file_folder = "{0}/{1}".format(DATA_FILE_PATH, mon_beg)
        mon_beg = mon_beg + 1
        for file in os.listdir(file_folder):  
            if not file.isdigit():
                continue
            path = os.path.join(file_folder, file)  
            if os.path.isdir(path): 
                date = int(file)
                file_full_path = "{0}/{1}_{2}.csv".format(path, CODE, date) 
                taget_full_path = "{0}/{1}_{2}.csv".format(path, "scl8", date) 
                if os.path.isfile(file_full_path):
                    os.rename(file_full_path, taget_full_path)
                    print("rename {0} {1}".format(file_full_path, taget_full_path))
                
main()                

