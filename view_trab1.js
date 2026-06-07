registerFileType((fileExt, filePath, fileData) => {
    return fileExt === 'bin';
});

registerParser((filePath) => {

    const isBTree = filePath.includes('BTree.bin');

    if (isBTree) {
        parseBTree();
    } else {
        parseArquivoNormal();
    }
});

function parseBTree(){
	registerParser(() => {

		setDefaults({
			"dark-colors": {
				"collapse": "var(--vscode-textLink-foreground)",
				"offset": "var(--vscode-descriptionForeground)",
				"size": "var(--vscode-descriptionForeground)",
				"name": "var(--vscode-foreground)",
				"value": "var(--vscode-editorInfo-foreground)",
				"description": "var(--vscode-descriptionForeground)",
				"row-header": "var(--vscode-sideBar-background)",
				"row-odd": "var(--vscode-editor-background)",
				"row-even": "var(--vscode-sideBar-background)"
			}
		});

		//Cabeçalho
		read(1);
		addRow('status', getStringValue(), '0 para inconsistente, 1 para consistente');
		read(4);
		addRow('noRaiz', getSignedNumberValue(), 'RRN do nó raíz');
		read(4);
		addRow('topo', getSignedNumberValue(), 'topo da pilha de removidos');	
		read(4);
		let proxRRN = getSignedNumberValue();
		addRow('proxRRN', proxRRN, '');
		read(4);
		addRow('nroNos', getSignedNumberValue(), '');

		//Dados
		let tamNomeEstacao, tamNomeLinha;
		for (let i = 0; i < proxRRN; i++) {
			read(53);
			addRow(`RRN ${i}`, '');
			addDetails(() => {
				read(1);
				addRow('removido', getStringValue(), '0 para não removido, 1 para removido');
				read(4);
				addRow('proximo', getSignedNumberValue(), '');
				read(4);
				addRow('tipoNo', getSignedNumberValue(), '-1 = folha, 0 = raíz, 1 = intermediário');
				read(4);
				addRow('nroChaves', getSignedNumberValue(), '');
				
				read(4);
				addRow('C1', getSignedNumberValue(), 'Chave 1');
				read(4);
				addRow('Pr1', getSignedNumberValue(), 'Offset chave 1');
				read(4);
				addRow('C2', getSignedNumberValue(), 'Chave 2');
				read(4);
				addRow('Pr2', getSignedNumberValue(), 'Offset chave 2');
				read(4);
				addRow('C3', getSignedNumberValue(), 'Chave 3');
				read(4);
				addRow('Pr3', getSignedNumberValue(), 'Offset chave 3');
				
				read(4);
				addRow('P1', getSignedNumberValue(), 'RRN da subárvore < C1');
				read(4);
				addRow('P2', getSignedNumberValue(), 'RRN da subárvore < C2');
				read(4);
				addRow('P3', getSignedNumberValue(), 'RRN da subárvore < C3');
				read(4);
				addRow('P4', getSignedNumberValue(), 'RRN da subárvore > C3');
			})
		}
	});
}

function parseArquivoNormal(){
	registerParser(() => {

	setDefaults({
        "dark-colors": {
            "collapse": "var(--vscode-textLink-foreground)",
            "offset": "var(--vscode-descriptionForeground)",
            "size": "var(--vscode-descriptionForeground)",
            "name": "var(--vscode-foreground)",
            "value": "var(--vscode-editorInfo-foreground)",
            "description": "var(--vscode-descriptionForeground)",
            "row-header": "var(--vscode-sideBar-background)",
            "row-odd": "var(--vscode-editor-background)",
            "row-even": "var(--vscode-sideBar-background)"
        }
    });

	//Cabeçalho
	read(1);
	addRow('status', getStringValue(), '0 para inconsistente, 1 para consistente');
	read(4);
	addRow('topo', getSignedNumberValue(), '-1 para nenhum removido');
	read(4);
	let proxRRN = getSignedNumberValue();
	addRow('proxRRN', proxRRN, '');
	read(4);
	addRow('nroEstacoes', getSignedNumberValue(), '');
	read(4);
	addRow('nroParesEstacao', getSignedNumberValue(), '');

	//Dados
	let tamNomeEstacao, tamNomeLinha;
	for (let i = 0; i < proxRRN; i++) {
		read(80);
		addRow(`RRN ${i}`, '');
		addDetails(() => {
			read(1);
			addRow('removido', getStringValue(), '0 para não removido, 1 para removido');
			read(4);
			addRow('proximo', getSignedNumberValue(), '-1 para não removido');
			read(4);
			addRow('codEstacao', getSignedNumberValue(), '');
			read(4);
			addRow('codLinha', getSignedNumberValue(), '');
			read(4);
			addRow('codProxEstacao', getSignedNumberValue(), '');
			read(4);
			addRow('distProxEstacao', getSignedNumberValue(), '');
			read(4);
			addRow('codLinhaIntegra', getSignedNumberValue(), '');
			read(4);
			addRow('codEstIntegra', getSignedNumberValue(), '');

			read(4);
			tamNomeEstacao = getSignedNumberValue();
			addRow('tamNomeEstacao', tamNomeEstacao, '');
			read(tamNomeEstacao);
			addRow('nomeEstacao', getStringValue(), '');

			read(4);
			tamNomeLinha = getSignedNumberValue();
			addRow('tamNomeLinha', tamNomeLinha, '');
			read(tamNomeLinha);
			addRow('nomeLinha', getStringValue(), '');

			read(43 - tamNomeEstacao - tamNomeLinha);
			addRow('lixo', getStringValue(), '');
		})
	}
});
}