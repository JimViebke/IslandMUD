#-----------------------------------------------------------
# Purpose:     Render IslandMUD hostile fortress partitions
#
# Author:      Jim Viebke
#
# Created:     Aug 17 2015
#-----------------------------------------------------------

print "importing..."

import matplotlib.pyplot as pyplot
import matplotlib.patches as patches

print "imports successful, working..."

partitions = [
[193, 809, 3, 3], [203, 809, 3, 4], [203, 813, 4, 4], [207, 813, 3, 4], [207, 817, 3, 3],
[210, 817, 3, 5], [210, 826, 3, 3], [193, 824, 4, 5], [207, 826, 3, 3], [210, 822, 3, 4],
[197, 824, 3, 5], [200, 824, 3, 5], [193, 815, 3, 4], [206, 809, 3, 4], [199, 815, 4, 3],
[209, 809, 4, 4], [199, 809, 4, 3], [203, 817, 4, 3], [207, 823, 3, 3], [196, 815, 3, 4],
[199, 821, 4, 3], [199, 812, 4, 3], [196, 819, 3, 5], [196, 809, 3, 3], [193, 819, 3, 5],
[199, 818, 4, 3], [196, 812, 3, 3], [193, 812, 3, 3], [210, 813, 3, 4], [207, 820, 3, 3],
[203, 820, 4, 6], [203, 826, 4, 3], ]
structures = [
[193, 809, 2, 3], [203, 809, 2, 4], [204, 814, 2, 2], [208, 814, 2, 3], [208, 817, 2, 3],
[210, 817, 3, 3], [210, 826, 3, 2], [194, 825, 2, 4], [208, 827, 2, 2], [210, 822, 2, 4],
[197, 824, 2, 4], [200, 824, 3, 4], [194, 817, 2, 2], [206, 809, 2, 3], [201, 815, 2, 2],
[210, 809, 3, 4], [201, 810, 2, 2], [204, 818, 3, 2], [207, 823, 2, 2], [196, 815, 2, 3],
[200, 821, 3, 2], [199, 812, 3, 3], [197, 819, 2, 4], [197, 810, 2, 2], [193, 820, 3, 3],
[199, 818, 4, 2], [196, 813, 2, 2], [194, 812, 2, 3], [210, 813, 3, 3], [207, 820, 3, 2],
[204, 820, 3, 6], [204, 826, 3, 2], ]

# get the fortress starting coordinates
min_0 = partitions[0][0]
min_1 = partitions[0][1]

# subtract the starting coordinates from each coordinate to move the
# fortress to 0,0
for partition in partitions:
    partition[0] -= min_0
    partition[1] -= min_1
for structure in structures:
    structure[0] -= min_0
    structure[1] -= min_1

# the width of the fortress, before downscaling
maximum_fortress_dimension = 50.0

# create a plot
fig6 = pyplot.figure()
ax6 = fig6.add_subplot(111, aspect='equal')

# for each structure
for structure in structures:
    # draw the structure to the plot
    ax6.add_patch(patches.Rectangle(
        (structure[0]/maximum_fortress_dimension, structure[1]/maximum_fortress_dimension),
        structure[2]/maximum_fortress_dimension, structure[3]/maximum_fortress_dimension,
        facecolor=('#0000FF'), linewidth=0.1
    ))

# for each partition
"""for partition in partitions:
    # draw the partition to the plot
    ax6.add_patch(patches.Rectangle(
        (partition[0]/maximum_fortress_dimension, partition[1]/maximum_fortress_dimension),
        partition[2]/maximum_fortress_dimension, partition[3]/maximum_fortress_dimension,
        facecolor=('none'), linewidth=2
    ))"""

# now draw a transparent grid on top of the plot
for i in range(0, int(maximum_fortress_dimension)):
    for j in range(0, int(maximum_fortress_dimension)):
        ax6.add_patch(patches.Rectangle(
            (i/maximum_fortress_dimension, j/maximum_fortress_dimension),
            1.0/maximum_fortress_dimension, 1.0/maximum_fortress_dimension,
            facecolor=('none'), linewidth=0.1
        ))

# save the image
print "saving image..."
fig6.savefig('D:/fortress.png', dpi=500, bbox_inches='tight')

print "finished"
























