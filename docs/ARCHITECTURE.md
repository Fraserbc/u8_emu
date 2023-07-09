# Instruction Operand Handling

The struct `u8_instr_oper` contains details on how to extract
the bits from the current instruction, which are then passed to
a handler function. This handler then returns a struct that can
be used to get and set the values of the operand.