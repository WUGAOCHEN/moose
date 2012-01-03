from options import *

test = { INPUT : 'internal_volume_test.i',
         CSVDIFF : ['out.csv']}

test_hex20 = { INPUT : 'internal_volume_hex20_test.i',
               PREREQ : 'test',
               CSVDIFF : ['out.csv']}

test_rz = { INPUT : 'internal_volume_rz_test.i',
            CSVDIFF : ['out_rz.csv']}

test_quad8_rz = { INPUT : 'internal_volume_rz_quad8_test.i',
                  PREREQ : 'test_rz',
                  CSVDIFF : ['out_rz.csv']}
