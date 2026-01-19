#!/bin/bash
BINARY=./build/bin/showbox
FIFO=/tmp/showbox_test_all.fifo

rm -f "${FIFO}"
mkfifo "${FIFO}"

${BINARY} <"${FIFO}" &
PID=$!

# Aguarda iniciar
sleep 1

cat >"${FIFO}" <<EOF
add window "Showcase Widgets" main width=1024 height=768
add tabwidget "Tabs" tabs

# Tab 1: Inputs
add page "Inputs" page1
add groupbox "Texto" gb1 horizontal=false
add label "Teste de Label com WordWrap habilitado para texto longo" lbl1 wordwrap=true
add lineedit "Texto Editavel" le1 placeholder="Digite..."
add textedit "Texto multilinha\nLine 2" te1
end
add groupbox "Seleção" gb2 horizontal=true
add checkbox "Check me" cb1 checked=true
add radiobutton "Option A" rb1 checked=true
add radiobutton "Option B" rb2
add combobox "Combo" combo1 items="Item A,Item B,Item C"
end
end

# Tab 2: Numeric
add page "Numeric" page2
add spinbox "Spin" sb1 value=50 minimum=0 maximum=100
add slider "Slider" sl1 value=30 minimum=0 maximum=100 horizontal=true
add progressbar "Progress" pb1 value=75 minimum=0 maximum=100 format="%p% Concluido"
end

# Tab 3: Data
add page "Data" page3
add listbox "Lista" lst1 items="Item 1,Item 2,Item 3" selection=true
add table "Tabela" tbl1 headers="Nome,Idade" rows="Alice,25;Bob,30"
end

# Tab 4: Chart
add page "Chart" page4
add chart "Vendas" chart1 title="Vendas Trimestrais" type=1 data="Q1:100,Q2:150,Q3:120,Q4:200"
end

end 
# End Tabs

show
EOF

echo "Interface renderizada. Verifique visualmente."
sleep 10
echo "Encerrando teste..."
rm "${FIFO}"
kill "${PID}"
