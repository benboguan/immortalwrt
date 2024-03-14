#!/bin/bash

RAW_OUTPUT=`switch reg r 0x7ffc | cut -d'=' -f3`
REG_VALUE=0x$RAW_OUTPUT
CHIP_ID=$[$REG_VALUE>>16]

function modify_100base()
{
    local board_name

    echo "7530 need fix 100base reg!"
    for i in {0..4}
    do
        echo "Set $i port"
        switch phy cl45 w $i 0x1e 0x1 0x01c0
        switch phy cl45 w $i 0x1e 0x7 0x03c0
        switch phy cl45 w $i 0x1e 0x4 0x209
        switch phy cl45 w $i 0x1e 0xA 0xc
    done

    for i in {0..4}
    do
        echo "Check $i port"
        switch phy cl45 r $i 0x1e 0x1
        switch phy cl45 r $i 0x1e 0x7
        switch phy cl45 r $i 0x1e 0x4
        switch phy cl45 r $i 0x1e 0xA
    done

    [ -f /tmp/sysinfo/board_name ] && board_name=$(cat /tmp/sysinfo/board_name)

    if [ "$board_name" = "mt7621-ax-nand-wax202" ]; then
        echo "Green on when link with 1000M!"
        switch phy cl45 w 0 0x1f 0x24 0xc001
        switch phy cl45 w 0 0x1f 0x25 0x3
    fi
}

function modify_collision()
{
    echo "7530 need modify collision reg!"
    local val=0x`switch reg r 30e0 | cut -d'=' -f3`
    local new_val=$[$val&0xE1FF]
    local hex=`printf "%x" $new_val`
    switch reg w 30e0 $hex
}

if [[ "$CHIP_ID" -eq 0x7530 ]];then
    modify_100base
    modify_collision
    resize
fi
