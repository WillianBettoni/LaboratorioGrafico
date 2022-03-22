//Inclusão de Bibliotecas
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

//Definição de variáveis
#define PI 3.1415
#define Triangulo 1
#define Retangulo 2
#define Circulo 3
#define AdicionaTriangulo 1
#define AdicionaRetangulo 2
#define AdicionaCirculo 3
#define TranslacaoUp 4
#define TranslacaoDown 5
#define TranslacaoLeft 6
#define TranslacaoRight 7
#define EscalaAdd 8
#define EscalaSub 9
#define RotacaoLeft 10
#define RotacaoRight 11
#define Clear 12
#define MoveUp 1
#define MoveDown 2
#define MoveRight 3
#define MoveLeft 4
#define aumentarEscala 5
#define diminuirEscala 6
#define rotacionarDireita 7
#define rotacionarEsquerda 8

//Componentes
HWND hAdicionaTriangulo;
HWND hAdicionaRetangulo;
HWND hAdicionaCirculo;
HWND hBaseTriangulo;
HWND hAlturaTriangulo;
HWND hComboCorTriangulo;
HWND hLarguraRetangulo;
HWND hAlturaRetangulo;
HWND hComboCorRetangulo;
HWND hRaioCirculo;
HWND hComboCorCirculo;
HWND hbuttonClear;
HWND hButtonUp;
HWND hButtonDown;
HWND hButtonLeft;
HWND hButtonRight;
HWND hButtonAdd;
HWND hButtonSub;
HWND hButtonRotLeft;
HWND hButtonRotRight;
HINSTANCE g_hinst;

//Definição de Estruturas
typedef struct comandos{
	int comando;
	struct comandos *proximo;
} ListaDeComandos;

typedef struct figurasGeometricas{
    int formaGeometrica;
    int cor;
    float altura;
    float base;
    float largura;
    float raio;
    ListaDeComandos *listaDeComandos;
    struct figurasGeometricas *proximo;
} InstanciasGeometricas;

//Esboço de Funções
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
void criarTriangulo(HWND hWnd);
void criarRetangulo(HWND hWnd);
void criarCirculo(HWND hWnd);
void desenharInstanciasGeometricas();
void aplicarListasTransformacoes(InstanciasGeometricas* formaGeometrica);
void mostrarFormasGeometrica(InstanciasGeometricas* instanciaGeometrica);
void desenharRetangulo(float largura, float altura, int cor);
void desenharTriangulo(float base, float altura, int cor);
void desenharCirculo(float raio, int cor);
void DrawWorld();
void addElements(HWND hWnd);
void DestruirComponentes();
InstanciasGeometricas* GetLastFormaGeometrica(InstanciasGeometricas *instancias);
InstanciasGeometricas* CreateTransformacao(InstanciasGeometricas *instancias, int comando);
ListaDeComandos* CreateInstanciaTransformacao(ListaDeComandos *transformacoes, int comando, ListaDeComandos *proximo);
ListaDeComandos* UltimoComando(ListaDeComandos *transformacoes);
InstanciasGeometricas *intanciasGeometricas = NULL;
InstanciasGeometricas* CreateInstanciaGeometrica(InstanciasGeometricas *instancias, int formaGeometrica, int cor, float altura, float base, float largura, float raio);
InstanciasGeometricas* CreateFormaGeometrica(InstanciasGeometricas *instancias, int formaGeometrica,int cor,float altura, float base, float largura,float raio, InstanciasGeometricas *proximo);

//Windowns API
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow){
  WNDCLASS wc = {};								//Declaração e Criação da Instância
  wc.lpszClassName = "My Window Class";			//Nome da Windowns Class
  wc.hbrBackground = (HBRUSH)COLOR_WINDOW;		//Cor de fundo da janela
  wc.lpfnWndProc = WindowProcedure;				//Window Procedure
  wc.hInstance = hInst;							//Instância da aplicação
  RegisterClass(&wc);
  
  g_hinst = hInst;

  HWND hWnd;
  hWnd = CreateWindow(
    wc.lpszClassName,
    "Formas Geométricas",
    WS_OVERLAPPEDWINDOW | WS_VISIBLE,
    0, 0, 800, 600,
    NULL, NULL, NULL, NULL
  );
  
  //Device Context
  HDC hDC;				
  hDC = GetDC(hWnd);
  
  //Pixel Format Descriptor
  PIXELFORMATDESCRIPTOR pfd;
  pfd.nSize        = sizeof(pfd);
  pfd.nVersion     = 1;
  pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | 0;
  pfd.iPixelType   = PFD_TYPE_RGBA;
  pfd.cColorBits   = 32;
  
  //Pixel Format
  int pf;
  pf = ChoosePixelFormat(hDC, &pfd);
  if (pf == 0) {
    MessageBox(NULL, "ChoosePixelFormat() failed", "Error", MB_ICONERROR); 
    return 0;
  } 
  if (SetPixelFormat(hDC, pf, &pfd) == FALSE) {
    MessageBox(NULL, "SetPixelFormat() failed.", "Error", MB_ICONERROR);
    return 0;
  }
  DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
  
  //OpenGL context
  HGLRC hRC;
  hRC = wglCreateContext(hDC);
  wglMakeCurrent(hDC, hRC);

  ShowWindow(hWnd, ncmdshow);
  
  MSG msg = {};
  while(GetMessage(&msg, NULL, 0, 0)){				//Loop para recuperar mensagem e armazenar em uma estrutura MSG
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  
  wglMakeCurrent(NULL, NULL);
  ReleaseDC(hWnd, hDC);
  wglDeleteContext(hRC);
  DestroyWindow(hWnd);
  
  return 0;
}

//Window Procedure
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp){	
	PAINTSTRUCT ps;
	
	switch (msg){
		case WM_PAINT:
      		addElements(hWnd);
      		DrawWorld();
      		BeginPaint(hWnd, &ps);
      		EndPaint(hWnd, &ps);
      		break;	
		case WM_COMMAND:
			switch(wp){
				case AdicionaTriangulo:
					criarTriangulo(hWnd);
					break;
				case AdicionaRetangulo:
					criarRetangulo(hWnd);
					break;
				case AdicionaCirculo:
					criarCirculo(hWnd);
					break;
				case TranslacaoUp:
					intanciasGeometricas = CreateTransformacao(
						intanciasGeometricas,
						MoveUp
					);	
					addElements(hWnd);
					break;
				case TranslacaoDown:
					intanciasGeometricas = CreateTransformacao(
						intanciasGeometricas,
						MoveDown
					);	
					addElements(hWnd);
					break;
				case TranslacaoLeft:
					intanciasGeometricas = CreateTransformacao(
						intanciasGeometricas,
						MoveLeft
					);	
					addElements(hWnd);
					break;
				case TranslacaoRight:
					intanciasGeometricas = CreateTransformacao(
						intanciasGeometricas,
						MoveRight
					);	
					addElements(hWnd);
					break;
				case EscalaAdd:
					intanciasGeometricas = CreateTransformacao(
						intanciasGeometricas,
						aumentarEscala
					);	
					addElements(hWnd);
					break;
				case EscalaSub:
					intanciasGeometricas = CreateTransformacao(
						intanciasGeometricas,
						diminuirEscala
					);	
					addElements(hWnd);
					break;
				case RotacaoLeft:
					intanciasGeometricas = CreateTransformacao(
						intanciasGeometricas,
						rotacionarDireita
					);	
					addElements(hWnd);
					break;
				case RotacaoRight:
					intanciasGeometricas = CreateTransformacao(
						intanciasGeometricas,
						rotacionarEsquerda
					);	
					addElements(hWnd);
					break;
				case Clear:
					glClear(GL_COLOR_BUFFER_BIT);
					intanciasGeometricas = NULL;
					PostMessage(hWnd, WM_PAINT, 0, 0);
					break;
        	}				
			break;	
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}

//Função criar Triângulo
void criarTriangulo(HWND hWnd) {	
	char textAltura[10], textBase[10];		
	GetWindowText(hBaseTriangulo, textBase, 10);	
	GetWindowText(hAlturaTriangulo, textAltura, 10);
	PostMessage(hWnd, WM_PAINT, 0, 0);	

	if(strtof(textBase, NULL) == 0 || strtof(textAltura, NULL) == 0){
		MessageBox(NULL, "Digite um valor maior que 0 para base e altura do triângulo!", "WARNING", MB_ICONWARNING);
		return;
	}	
	intanciasGeometricas = CreateInstanciaGeometrica(
		intanciasGeometricas, 
		Triangulo, 
		SendMessage(hComboCorTriangulo, CB_GETCURSEL, 0, 0), 
		strtof(textAltura, NULL), 
		strtof(textBase, NULL), 
		0.0f, 
		0.0f
	);	
	addElements(hWnd);
}

//Função criar Retângulo
void criarRetangulo(HWND hWnd) {
	
    char textAltura[10], textLargura[10];
	
	GetWindowText(hLarguraRetangulo, textLargura, 10);	
	GetWindowText(hAlturaRetangulo, textAltura, 10);
	PostMessage(hWnd, WM_PAINT, 0, 0);
		
	if(strtof(textLargura, NULL) == 0 || strtof(textAltura, NULL) == 0){
		MessageBox(NULL, "Digite um valor maior que 0 para largura e altura do retângulo!", "WARNING", MB_ICONWARNING);
		return;
	}
	intanciasGeometricas = CreateInstanciaGeometrica(
		intanciasGeometricas, 
		Retangulo, 
		SendMessage(hComboCorRetangulo, CB_GETCURSEL, 0, 0), 
		strtof(textAltura, NULL), 
		0.0f, 
		strtof(textLargura, NULL), 
		0.0f
	);	
	addElements(hWnd);
}

//Função criar Círculo
void criarCirculo(HWND hWnd) {
    char textRaio[10];
	GetWindowText(hRaioCirculo, textRaio, 10);
	PostMessage(hWnd, WM_PAINT, 0, 0);	
	
	if(strtof(textRaio, NULL) == 0){
		MessageBox(NULL, "Digite um valor maior que 0 para o raio do círculo!", "WARNING", MB_ICONWARNING);
		return;
	}	
	intanciasGeometricas = CreateInstanciaGeometrica(
		intanciasGeometricas, 
		Circulo, 
		SendMessage(hComboCorCirculo, CB_GETCURSEL, 0, 0), 
		0.0f, 
		0.0f, 
		0.0f, 
		strtof(textRaio, NULL)
	);
	addElements(hWnd);
}

//Função adicionar os elementos na Window Class
void addElements(HWND hwnd){
	const wchar_t *items[] = { L"Vermelho", L"Verde", L"Azul"};
	
	DestruirComponentes();
	
	//Título
	CreateWindow(
		"Static",
		"Formas Geométricas",
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		20, 20, 280, 20,
		hwnd,
		NULL, NULL, NULL
	);
	
	//Triângulo
	CreateWindow(
		"Static",
		"Base",
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		20, 50, 80, 20,
		hwnd,
		NULL, NULL, NULL
	);
		
	hBaseTriangulo = CreateWindow(
		"Edit",
		"",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
		20, 75, 80, 24.5,
		hwnd,
		NULL, NULL, NULL
	);
	
	CreateWindow(
		"Static",
		"Altura",
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		110, 50, 80, 20,
		hwnd,
		NULL, NULL, NULL
	);
	
	hAlturaTriangulo = CreateWindow(
		"Edit",
		"",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
		110, 75, 80, 24.5,
		hwnd,
		NULL, NULL, NULL
	);
	
	CreateWindow(
		"Static",
		"Cor",
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		200, 50, 100, 20,
		hwnd,
		NULL, NULL, NULL
	);
	
	hComboCorTriangulo = CreateWindowW(
		L"Combobox", NULL, 
    	WS_CHILD | WS_VISIBLE | CBS_DROPDOWN,
    	200, 75, 100, 80, hwnd, NULL, g_hinst, NULL
	);
		
	for (int i = 0; i < 3; i++ ) {              
    	SendMessageW(hComboCorTriangulo, CB_ADDSTRING, 0, (LPARAM) items[i]);
    }  
    SendMessageW(hComboCorTriangulo, CB_SETCURSEL, 0, (LPARAM) 0);
    
	hAdicionaTriangulo = CreateWindow(
		"Button",
		"Adicionar Triângulo",
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		20, 110, 175, 40,
		hwnd,
		(HMENU)AdicionaTriangulo, NULL, NULL
	);

	//Retângulo
	CreateWindow(
		"Static",
		"Largura",
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		20, 180, 80, 20,
		hwnd,
		NULL, NULL, NULL
	);
	
	hLarguraRetangulo = CreateWindow(
		"Edit",
		"",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
		20, 205, 80, 24.5,
		hwnd,
		NULL, NULL, NULL
	);
	
	CreateWindow(
		"Static",
		"Altura",
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		110, 180, 80, 20,
		hwnd,
		NULL, NULL, NULL
	);
	
	hAlturaRetangulo = CreateWindow(
		"Edit",
		"",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
		110, 205, 80, 24.5,
		hwnd,
		NULL, NULL, NULL
	);
	
	CreateWindow(
		"Static",
		"Cor",
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		200, 180, 100, 20,
		hwnd,
		NULL, NULL, NULL
	);
		
	hComboCorRetangulo = CreateWindowW(
		L"Combobox", NULL, 
    	WS_CHILD | WS_VISIBLE | CBS_DROPDOWN,
    	200, 205, 100, 80, hwnd, NULL, g_hinst, NULL
	);
		
	for (int i = 0; i < 3; i++ ) {              
    	SendMessageW(hComboCorRetangulo, CB_ADDSTRING, 0, (LPARAM) items[i]);
    }  
    SendMessageW(hComboCorRetangulo, CB_SETCURSEL, 0, (LPARAM) 0);
    
	hAdicionaRetangulo = CreateWindow(
		"Button",
		"Adicionar Retângulo",
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		20, 240, 175, 40,
		hwnd,
		(HMENU)AdicionaRetangulo, NULL, NULL
	);
	
	//Circulo
	CreateWindow(
		"Static",
		"Raio",
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		20, 310, 175, 20,
		hwnd,
		NULL, NULL, NULL
	);
	
	hRaioCirculo = CreateWindow(
		"Edit",
		"",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
		20, 335, 175, 24.5,
		hwnd,
		NULL, NULL, NULL
	);
	
	CreateWindow(
		"Static",
		"Cor",
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		200, 310, 100, 20,
		hwnd,
		NULL, NULL, NULL
	);
	
	hComboCorCirculo = CreateWindowW(
		L"Combobox", NULL, 
    	WS_CHILD | WS_VISIBLE | CBS_DROPDOWN,
    	200, 335, 100, 80, hwnd, NULL, g_hinst, NULL
	);
		
	for (int i = 0; i < 3; i++ ) {              
    	SendMessageW(hComboCorCirculo, CB_ADDSTRING, 0, (LPARAM) items[i]);
    }  
    SendMessageW(hComboCorCirculo, CB_SETCURSEL, 0, (LPARAM) 0);
    
	hAdicionaCirculo = CreateWindow(
		"Button",
		"Adicionar Circulo",
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		20, 370, 175, 40,
		hwnd,
		(HMENU)AdicionaCirculo, NULL, NULL
	);
	
	//Translação
	CreateWindow(
		"Static",
		"Translação",
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		20, 460, 100, 20,
		hwnd,
		NULL, NULL, NULL
	);
	
		hButtonUp = CreateWindow(
		"button",
		"^",
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		55, 485, 30, 30,
		hwnd,
		(HMENU)TranslacaoUp, NULL, NULL
	);
		
	hButtonLeft = CreateWindow(
		"Button",
		"<",
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		20, 520, 30, 30,
		hwnd,
		(HMENU)TranslacaoLeft, NULL, NULL
	);		
	
	hButtonDown = CreateWindow(
		"Button",
		"v",
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		55, 520, 30, 30,
		hwnd,
		(HMENU)TranslacaoDown, NULL, NULL
	);
	
	hButtonRight = CreateWindow(
		"Button",
		">",
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		90, 520, 30, 30,
		hwnd,
		(HMENU)TranslacaoRight, NULL, NULL
	);

	//Escala
	CreateWindow(
		"Static",
		"Escala",
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		148, 460, 60, 20,
		hwnd,
		NULL, NULL, NULL
	);
		
	hButtonAdd = CreateWindow(
		"Button",
		"+",
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		163, 485, 30, 30,
		hwnd,
		(HMENU)EscalaAdd, NULL, NULL
	);
	
	hButtonSub = CreateWindow(
		"Button",
		"-",
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		163, 520, 30, 30,
		hwnd,
		(HMENU)EscalaSub, NULL, NULL
	);
	
	//Rotação
	CreateWindow(
		"Static",
		"Rotação",
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		235, 460, 60, 20,
		hwnd,
		NULL, NULL, NULL
	);
		
	hButtonRotRight = CreateWindow(
		"Button",
		">",
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		250, 485, 30, 30,
		hwnd,
		(HMENU)RotacaoRight, NULL, NULL
	);
	
	hButtonRotLeft = CreateWindow(
		"Button",
		"<",
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		250, 520, 30, 30,
		hwnd,
		(HMENU)RotacaoLeft, NULL, NULL
	);	
	
	//Clear
	hbuttonClear = CreateWindow(
		"Button",
		"Limpar Formas",
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		20, 415, 175, 40,
		hwnd,
		(HMENU)Clear, NULL, NULL
	);
	
	DrawWorld();
}

//Função DrawWorld OpenGL
void DrawWorld(){
	glViewport(400, 0, 400, 600);
  	glBegin(GL_QUADS);
  	glEnd();  	 	
	glMatrixMode(GL_MODELVIEW);
   	glLoadIdentity();     	
  	glClear(GL_COLOR_BUFFER_BIT);  	
  	desenharInstanciasGeometricas();
  	glFlush();
}

//Função desenha instancias Geométricas
void desenharInstanciasGeometricas() {
	InstanciasGeometricas *atual = intanciasGeometricas;
	while(atual != NULL) {
		glPushMatrix();
		glMatrixMode(GL_MODELVIEW);		
		aplicarListasTransformacoes(atual);
		mostrarFormasGeometrica(atual);		
		atual = atual->proximo;		
		glPopMatrix();
	}
}

//Função Translação, Escala e Rotação
void aplicarListasTransformacoes(InstanciasGeometricas* formaGeometrica) {
	ListaDeComandos *transformacoes = formaGeometrica->listaDeComandos;
	int comando; 	
	while(transformacoes != NULL) {
		comando = transformacoes->comando;
		switch(comando) {
			case MoveUp:
				glTranslatef(0.0f, 0.01f, 0.0f);
				break;
			case MoveDown:
				glTranslatef(0.0f, -0.01f, 0.0f);
				break;
			case MoveRight:
				glTranslatef(0.01f, 0.0f, 0.0f);
				break;
			case MoveLeft:
				glTranslatef(-0.01f, 0.0f, 0.0f);
				break;
			case aumentarEscala:
				glScalef(1.1, 1.1, 0);
				break;
			case diminuirEscala:
				glScalef(0.9, 0.9, 0);
				break;
			case rotacionarDireita:
				glRotatef(1, 0, 0, 1.0);
				break;
			case rotacionarEsquerda:
				glRotatef(-1, 0, 0, 1.0);
				break;
		}		
		transformacoes = transformacoes->proximo;
	}
}

//Função desenha Formas Geométricas
void mostrarFormasGeometrica(InstanciasGeometricas* instanciaGeometrica) {
	switch(instanciaGeometrica->formaGeometrica) {
		case Triangulo:
			desenharTriangulo(instanciaGeometrica->base, instanciaGeometrica->altura, instanciaGeometrica->cor);
			break;
		case Retangulo:
			desenharRetangulo(instanciaGeometrica->largura, instanciaGeometrica->altura, instanciaGeometrica->cor);
			break;
		case Circulo:
			desenharCirculo(instanciaGeometrica->raio, instanciaGeometrica->cor);
			break;
	}
}

//Desenha Triângulo
void desenharTriangulo(float base, float altura, int cor){	
	glBegin(GL_TRIANGLES);	
	switch(cor){
		case 0:
			glColor3f(1.0f, 0.0f, 0.0f);
			break;
		case 1:
			glColor3f(0.0f, 1.0f, 0.0f);
			break;
		case 2:
			glColor3f(0.0f, 0.0f, 1.0f);
			break;
	}
    glVertex2f(-0.5, -0.5);
    glVertex2f((base*0.1)-0.5, -0.5);
    glVertex2f((((base*0.1)/2)-0.5), (altura*0.1)-0.5);    
	glEnd();					
	glFlush();	
}

//Desenha Retângulo
void desenharRetangulo(float largura, float altura, int cor){		
	glBegin(GL_QUADS);
	switch(cor){
		case 0:
			glColor3f(1.0f, 0.0f, 0.0f);
			break;
		case 1:
			glColor3f(0.0f, 1.0f, 0.0f);
			break;
		case 2:
			glColor3f(0.0f, 0.0f, 1.0f);
			break;
	}
	glVertex2f(-0.5, -0.5);
    glVertex2f((largura*0.1)-0.5, -0.5);
    glVertex2f((largura*0.1)-0.5, (altura*0.1)-0.5);
    glVertex2f(-0.5, (altura*0.1)-0.5);	    
	glEnd();					
	glFlush();	
}

//Desenha Círculo
void desenharCirculo(float raio, int cor){	
	int i;	
	GLint vertices = 100;	
	GLfloat angulo;
	glBegin(GL_POLYGON);
	switch(cor){
		case 0:
			glColor3f(1.0f, 0.0f, 0.0f);
			break;
		case 1:
			glColor3f(0.0f, 1.0f, 0.0f);
			break;
		case 2:
			glColor3f(0.0f, 0.0f, 1.0f);
			break;
	}		
	for(int i = 0; i < vertices; i++) { 
		angulo = i * 2 * PI / vertices; 
		glVertex2f((cos(angulo) * (raio * 0.1)), (sin(angulo) * (raio * 0.1)));
	}
	glEnd();			
	glFlush();	
}

InstanciasGeometricas* CreateInstanciaGeometrica(InstanciasGeometricas *instancias, int formaGeometrica, int cor, float altura, float base, float largura, float raio){
    InstanciasGeometricas *novo;
    InstanciasGeometricas *atual;
    if (instancias == NULL){
        return CreateFormaGeometrica(instancias, formaGeometrica, cor, altura, base, largura, raio, NULL);
    }
    atual = GetLastFormaGeometrica(instancias);
    novo = CreateFormaGeometrica(instancias, formaGeometrica, cor, altura, base, largura, raio, NULL);
    atual->proximo = novo;
    return instancias;
}

InstanciasGeometricas* GetLastFormaGeometrica(InstanciasGeometricas *instancias){
	InstanciasGeometricas* p = instancias;
    while (p->proximo != NULL) {
        p = p->proximo;
    }
    return p;
}

InstanciasGeometricas* CreateFormaGeometrica(InstanciasGeometricas *instancias, int formaGeometrica, int cor, float altura, float base, float largura, float raio, InstanciasGeometricas *proximo){
	InstanciasGeometricas* novo = (InstanciasGeometricas *) malloc(sizeof(InstanciasGeometricas));
    novo->formaGeometrica = formaGeometrica;
    novo->cor = cor;
    novo->altura = altura;
    novo->base = base;
    novo->largura = largura;
    novo->raio = raio;
    novo->listaDeComandos = NULL;
    novo->proximo = proximo;
    return novo;
}

InstanciasGeometricas* CreateTransformacao(InstanciasGeometricas *instancias, int comando){	
	InstanciasGeometricas *ultimaInstancia = GetLastFormaGeometrica(instancias);
	ListaDeComandos *transformacoes = ultimaInstancia->listaDeComandos;	
	ListaDeComandos *novo;
    ListaDeComandos *atual;
    if (transformacoes == NULL) {
    	transformacoes = CreateInstanciaTransformacao(transformacoes, comando, NULL);
    	ultimaInstancia->listaDeComandos = transformacoes;
    } else {
    	atual = UltimoComando(transformacoes);
		novo = CreateInstanciaTransformacao(transformacoes, comando, NULL);
		atual->proximo = novo;
	}    
    return instancias;
}

ListaDeComandos* CreateInstanciaTransformacao(ListaDeComandos *transformacoes, int comando, ListaDeComandos *proximo){
	ListaDeComandos* novo = (ListaDeComandos *) malloc(sizeof(ListaDeComandos));
    novo->comando = comando;
    novo->proximo = proximo;
    return novo;
}

ListaDeComandos* UltimoComando(ListaDeComandos *transformacoes){
	ListaDeComandos* p = transformacoes;
    while (p->proximo != NULL) {
        p = p->proximo;
    }
    return p;
}

//Função destroí Componentes
void DestruirComponentes(){	
	DestroyWindow(hAdicionaTriangulo);
	DestroyWindow(hAdicionaRetangulo);
	DestroyWindow(hAdicionaCirculo);
	DestroyWindow(hBaseTriangulo);
	DestroyWindow(hAlturaTriangulo);
	DestroyWindow(hComboCorTriangulo);
	DestroyWindow(hComboCorRetangulo);
	DestroyWindow(hComboCorCirculo);
	DestroyWindow(hLarguraRetangulo);
	DestroyWindow(hAlturaRetangulo);
	DestroyWindow(hButtonAdd);
	DestroyWindow(hButtonSub);
	DestroyWindow(hRaioCirculo);
	DestroyWindow(hButtonUp);
	DestroyWindow(hButtonLeft);
	DestroyWindow(hButtonRight);
	DestroyWindow(hButtonDown);
	DestroyWindow(hButtonRotRight);
	DestroyWindow(hButtonRotLeft);
}
