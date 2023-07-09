import re

masks = ["1000_nnnn_mmmm_0001",
"0001_nnnn_iiii_iiii",
"1111_nnn0_mmm0_0110",
"1110_nnn0_1iii_iiii",
"1000_nnnn_mmmm_0110",
"0110_nnnn_iiii_iiii",
"1000_nnnn_mmmm_0010",
"0010_nnnn_iiii_iiii",
"1000_nnnn_mmmm_0111",
"0111_nnnn_iiii_iiii",
"1000_nnnn_mmmm_0101",
"0101_nnnn_iiii_iiii",
"1111_nnn0_mmm0_0101",
"1110_nnn0_0iii_iiii",
"1000_nnnn_mmmm_0000",
"0000_nnnn_iiii_iiii",
"1000_nnnn_mmmm_0011",
"0011_nnnn_iiii_iiii",
"1000_nnnn_mmmm_0100",
"0100_nnnn_iiii_iiii",
"1111_nnn0_mmm0_0111",
"1000_nnnn_mmmm_1000",
"1000_nnnn_mmmm_1001",
"1000_nnnn_mmmm_1010",
"1001_nnnn_0www_1010",
"1000_nnnn_mmmm_1011",
"1001_nnnn_0www_1011",
"1000_nnnn_mmmm_1110",
"1001_nnnn_0www_1110",
"1000_nnnn_mmmm_1100",
"1001_nnnn_0www_1100",
"1000_nnnn_mmmm_1101",
"1001_nnnn_0www_1101",
"1001_nnn0_0011_0010",
"1001_nnn0_0101_0010",
"1001_nnn0_mmm0_0010",
"1010_nnn0_mmm0_1000",
"1011_nnn0_00DD_DDDD",
"1011_nnn0_01DD_DDDD",
"1001_nnn0_0001_0010",
"1001_nnnn_0011_0000",
"1001_nnnn_0101_0000",
"1001_nnnn_mmm0_0000",
"1001_nnnn_mmm0_1000",
"1101_nnnn_00DD_DDDD",
"1101_nnnn_01DD_DDDD",
"1001_nnnn_0001_0000",
"1001_nn00_0011_0100",
"1001_nn00_0101_0100",
"1001_n000_0011_0110",
"1001_n000_0101_0110",
"1001_nnn0_0011_0011",
"1001_nnn0_0101_0011",
"1001_nnn0_mmm0_0011",
"1010_nnn0_mmm0_1001",
"1011_nnn0_10DD_DDDD",
"1011_nnn0_11DD_DDDD",
"1001_nnn0_0001_0011",
"1001_nnnn_0011_0001",
"1001_nnnn_0101_0001",
"1001_nnnn_mmm0_0001",
"1001_nnnn_mmm0_1001",
"1101_nnnn_10DD_DDDD",
"1101_nnnn_11DD_DDDD",
"1001_nnnn_0001_0001",
"1001_nn00_0011_0101",
"1001_nn00_0101_0101",
"1001_n000_0011_0111",
"1001_n000_0101_0111",
"1110_0001_iiii_iiii",
"1010_0000_mmmm_1111",
"1010_mmm0_0000_1101",
"1010_0000_mmmm_1100",
"1010_nnn0_0000_0101",
"1010_nnn0_0001_1010",
"1010_0000_mmmm_1011",
"1110_1001_iiii_iiii",
"1010_nnnn_0000_0111",
"1010_nnnn_0000_0100",
"1010_nnnn_0000_0011",
"1010_0001_mmm0_1010",
"1111_nnn0_0101_1110",
"1111_n000_0111_1110",
"1111_nnnn_0100_1110",
"1111_nn00_0110_1110",
"1111_lepa_1100_1110",
"1111_nnn0_0001_1110",
"1111_n000_0011_1110",
"1111_nnnn_0000_1110",
"1111_nn00_0010_1110",
"1111_lepa_1000_1110",
"1010_nnnn_mmmm_1110",
"1111_nnn0_0010_1101",
"1111_nnn0_0011_1101",
"1111_nnnn_0000_1101",
"1111_nnnn_0001_1101",
"1111_nn00_0100_1101",
"1111_nn00_0101_1101",
"1111_n000_0110_1101",
"1111_n000_0111_1101",
"1010_nnnn_mmmm_0110",
"1111_mmm0_1010_1101",
"1111_mmm0_1011_1101",
"1111_mmmm_1000_1101",
"1111_mmmm_1001_1101",
"1111_mm00_1100_1101",
"1111_mm00_1101_1101",
"1111_m000_1110_1101",
"1111_m000_1111_1101",
"1111_0000_mmm0_1010",
"1111_0000_mmm0_1011",
"1111_0000_0000_1100",
"1000_nnnn_0001_1111",
"1000_nnnn_0011_1111",
"1000_nnnn_0101_1111",
"1010_nnnn_0bbb_0000",
"1010_0000_1bbb_0000",
"1010_nnnn_0bbb_0010",
"1010_0000_1bbb_0010",
"1010_nnnn_0bbb_0001",
"1010_0000_1bbb_0001",
"1110_1101_0000_1000",
"1110_1011_1111_0111",
"1110_1101_1000_0000",
"1110_1011_0111_1111",
"1111_1110_1100_1111",
"1100_0000_rrrr_rrrr",
"1100_0001_rrrr_rrrr",
"1100_0010_rrrr_rrrr",
"1100_0011_rrrr_rrrr",
"1100_0100_rrrr_rrrr",
"1100_0101_rrrr_rrrr",
"1100_0110_rrrr_rrrr",
"1100_0111_rrrr_rrrr",
"1100_1000_rrrr_rrrr",
"1100_1001_rrrr_rrrr",
"1100_1010_rrrr_rrrr",
"1100_1011_rrrr_rrrr",
"1100_1100_rrrr_rrrr",
"1100_1101_rrrr_rrrr",
"1100_1110_rrrr_rrrr",
"1000_nnn1_nnn0_1111",
"1110_0101_00ii_iiii",
"1111_1111_1111_1111",
"1111_gggg_0000_0000",
"1111_0000_nnn0_0010",
"1111_gggg_0000_0001",
"1111_0000_nnn0_0011",
"1111_nnn0_mmmm_0100",
"1111_nnn0_mmmm_1001",
"1111_1110_0010_1111",
"1111_1110_0011_1111",
"1111_1110_0001_1111",
"1111_1110_0000_1111",
"1111_1110_1000_1111"]

for m in masks:
	# Remove underscores
	m = re.sub("_", "", m)

	# Generate the instruction mask
	instr_mask = re.sub("0", "1", m)
	instr_mask = re.sub("[^1]", "0", instr_mask)
	instr_mask = int(instr_mask, 2)

	instr_val = re.sub("[^01]", "0", m)
	instr_val = int(instr_val, 2)

	# Extract the arguments
	args = []
	for x in m:
		if x in "01": continue
		if x not in args: args.append(x)
	
	if len(args) > 2:
		print(f"{instr_mask:04x} Do the args mask yourself")
		continue

	print(f"{{instr_mask=0x{instr_mask:04x}, instr_val=0x{instr_val:04x}, ", end="")

	while len(args) != 2: args.append(0)
	for x in range(len(args)):
		arg = args[x]
		if arg != 0:
			arg_mask = re.sub(f"[^{arg}]", "0", m)
			arg_mask = re.sub(f"{arg}", "1", arg_mask)
			arg_mask = int(arg_mask, 2)
		else:
			arg_mask = 0

		args[x] = f"arg{x}_mask=0x{arg_mask:04x}"

	print(", ".join(args), end="")
	print("},")