#!/bin/false


#   ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
#   ┃   𝐓𝐡𝐢𝐬 𝐢𝐬 𝐭𝐡𝐞 𝐟𝐢𝐥𝐞 𝐲𝐨𝐮 𝐚𝐫𝐞 𝐥𝐨𝐨𝐤𝐢𝐧𝐠 𝐟𝐨𝐫   ┃
#   ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛


# NOTE | See ‹tests-readme.md› on how to add your own tests to this suite or how
#      | to create your own suite.


create_flat_tree() {
	create_path root

	for i in $(seq 1 10); do
		create_file root/file"$i" $((i * i * 14 - 2 * i + 7))
	done
}

test_basic() {
	create_flat_tree
	du root
}

run "Plochá struktura" test_basic


create_cmp_tree() {
	create_path root

	for file in AA Aa aA aa Ba; do
		create_file root/"$file" 64
	done
}

test_cmp_tree() {
	create_cmp_tree
	du root
}

run "Řazení souborů" test_cmp_tree


test_apparent() {
	create_flat_tree
	du -a root
}

run "Plochá struktura s přepínačem '-a'" test_apparent


create_shallow_tree() {
	create_flat_tree
	create_path root/bin
	create_path root/lib

	create_file root/bin/ls 4578
	create_file root/bin/tar 97687

	create_file root/lib/libc.a 679854
}

test_shallow() {
	create_shallow_tree
	du -a root
}

run "Mělká struktura hloubky 2 s přepínačem '-a'" test_shallow


create_hidden() {
	create_flat_tree
	create_file root/.devil 666
}

test_hidden() {
	create_hidden
	du root
}

run "Plochá struktura se skrytým souborem" test_hidden


create_inaccessible() {
	create_shallow_tree
	create_path root/secrets
	chmod a= root/secrets
}

test_inaccessible() {
	create_inaccessible
	du root
}

run "Mělká struktura s nepřístupným adresářem" test_inaccessible


# NOTE | See ‹tests-readme.md› on how to add your own tests to this suite or how
#      | to create your own suite.

