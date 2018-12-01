#Report for mergesort

- i have created a test case generator file with name test_generator
- give input n and it will create a file with n random generated numbers
- and also a help.sh file to help us in making report

#time taken by each Mergesort with n
"""
n       | normal mergesort      | process mergesort        | thread mergesort          |
10      |  0.000005s            | 0.000077s                | 0.000144                  |
100     |  0.000016             | 0.000129                 | 0.005540                  |
1000    |  0.000165             | 0.000119                 | 0.030578                  |
10000   |  0.001961             | 0.000220                 | 0.229954                  |
100000  |  0.022590             | 0.001090                 | seg fault                 |
1000000 |  0.247786             | 0.009149                 | seg fault                 |
"""