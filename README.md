# miniRV
在一生一芯f6阶段，对miniRV的八条命令进行手工译码存在较大工作量，该程序可以完成这一工作
**命令需要满足一定的格式要求，如下：**
- add rd,rs1,rs2
- addi rd,rs1,imm
- lui rd,imm
- jalr rd,offset(rs1)
- sw rs2,offset(rs1)
- sb rs2,offset(rs1)
- lw rd,offset(rs1)
- lbu rd,offset(rs1)
