#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#define MAX(i, j) (((i) > (j)) ? (i) : (j))

//initializing the image size, we will modify it when we read the file
int size = 600;

struct Composante
{
    unsigned char n;
};
typedef struct Composante Composante;

//Let a color be defined by an array of its values of, respectively, Red, Green and Blue
struct Couleur
{
    Composante rgb[3];
};
typedef struct Couleur Couleur;

struct Opacite
{
    Composante alpha;
};
typedef struct Opacite Opacite;

//Let a pixel be defined by a Color and an Opacity
struct Pixel
{
    Couleur couleur;
    Opacite opacite;
};
typedef struct Pixel Pixel;

//Let a calc be defined by its size and a 2D array of Pixels
struct Calque
{
    int taille;
    Pixel** grille;
};
typedef struct Calque Calque;

//Let a paint bucket be defined by a linked list of Colors.
typedef struct Couche_Couleur* Seau_Couleur;
struct Couche_Couleur{
    Couleur couleur;
    Seau_Couleur next;
};

//Let an opacity bucket be defined by a linked list of Opacities
typedef struct Couche_Opacite* Seau_Opacite;
struct Couche_Opacite{
    Opacite opacite;
    Seau_Opacite next;
};

struct Pile_Calques{
    Calque contenu[10];
    int top;
};
typedef struct Pile_Calques Pile_Calques;

//Let the position be defined by a couple of coordinates
struct Position{
    int x;
    int y;
};
typedef struct Position Position;

//We define an explicit type for the stack of positions left to go throught
struct Noeud{
Position position;
struct Noeud* next;
};
typedef struct Noeud* Pile;

/*
@requires a Position and a Stack of positions to go throught
@ensures pushes the Position to the top of the Stack
@returns nothing
*/
void push(Position p, Pile *s)
{
    Pile new_pile = malloc(sizeof(struct Noeud));
    new_pile->position = p;
    new_pile->next = *s;
    *s = new_pile;
}
/*
@requires a Stack of positions to go throught
@ensures Removes the top Position of the stack if the stack is not empty, does nothing if the stack is empty
@returns the removed top Position or {0,0} in the case the stack is empty
*/
Position pop(Pile *s)
{
    if (*s == NULL)
    {
        printf("la pile est vide\n");
        Position res = {0,0};
        return res;
    }
    else
    {
        Pile temp;
        temp = *s;
        *s = (*s)->next;
        Position res = temp->position;
        free(temp);
        return res;
    }
}

/*
@requires 2 Pixels
@ensures Checks if the given Pixels are the same
@returns 1 if said Pixels are the same, 0 if not
*/
int same_pixel(Pixel p1, Pixel p2)
{
    if (p1.opacite.alpha.n != p2.opacite.alpha.n)
    {
    return 0;
    }
    for (int i=0; i<3; i+=1)
    {
        if (p1.couleur.rgb[i].n != p2.couleur.rgb[i].n)
        return 0;

    }
    return 1;
}




/*
@requires Values of Red, Gree and Blue
@ensures Creates a Color with matching values of Red, Green and Blue
@returns The created Color
*/
Couleur creer_couleur(unsigned int r, unsigned int g, unsigned int b)
{
    Couleur col;
    col.rgb[0].n = r;
    col.rgb[1].n = g;
    col.rgb[2].n = b;
    return col;
}

/*
@requires An image size
@ensures Allocates memory of size corresponding to the size passed in argument
@returns An empty Calc of the corresponding size
*/
Calque creer_calque(int w)
{
    Calque clq;
    Opacite transparent; transparent.alpha.n = 0;
    clq.taille = w;
    clq.grille=(Pixel **)calloc(w,sizeof(Pixel *));
    for(int i=0; i<w; i+=1)
    {
        clq.grille[i]=(Pixel*)calloc(w,sizeof(Pixel));
        clq.grille[i]->couleur = creer_couleur(0,0,0);
        clq.grille[i]->opacite = transparent;
    }
    return clq;
}

/*
@requires nothing
@ensures Creates an empty Calc stack
@returns The created stack
*/
Pile_Calques creer_pile_calques()
{
    Pile_Calques pc;
    pc.top =-1;
    return pc;
}

/*
@requires A Calc stack and a Calc
@ensures Adds the Calc to the top of the Calc stack
@returns
*/
void empile_calque(Pile_Calques *pc, Calque item)
{
    if (pc->top < 9)
    {
        pc->top+=1;
        pc->contenu[pc->top] = item;
    }
}

/*
@requires A Calc stack
@ensures Removes the top Calc of the stack if it is not empty, does nothing if the stack is empty
@returns The removed Calc if the stack was not empty, an empty Calc if not
*/
Calque pop_calque(Pile_Calques *pc)
{
    if (pc->top > -1)
    {
        pc->top-=1;
        return pc->contenu[pc->top+1];
    }
    else
    {
        printf("Stack is empty");
        return creer_calque(size);
    }
}

/*
@requires A Pixel
@assigns Prints the Pixel values as "red value, green value, blue value, opacity value"
@returns Nothing
*/
void print_pixel(Pixel pix)
{
   printf("r : %d ; g : %d ; b : %d ; o : %d ; ", pix.couleur.rgb[0].n, pix.couleur.rgb[1].n, pix.couleur.rgb[2].n, pix.opacite.alpha.n);
}

/*
@requires A Color and the Color Bucket
@ensures Adds the Color to the Bucket and allows memory for the bucket if the bucket is NULL
@returns Nothing
*/
void ajout_c(Couleur c, Seau_Couleur *sc)
{
    if (*sc == NULL)
    {
        *sc = malloc(sizeof(struct Couche_Couleur));
        (*sc)->couleur=c;
        (*sc)->next = NULL;
    }
    else
    {
    Seau_Couleur nouveau_seau = NULL;
    ajout_c(c,&nouveau_seau);
    //This recursive call ends as nouveau_seau is NULL so the first recursive call will end up in the if condition
    nouveau_seau->next  = *sc;
    *sc = nouveau_seau;
    }
}

/*
@requires An Opacity and an Opacity Bucket
@ensures Adds the Opacity to the Bucket and allows memory for the bucket if the bucket is NULL
@returns Nothing
*/
void ajout_o(Opacite o, Seau_Opacite *so)
{
    if (*so == NULL)
    {
        *so = malloc(sizeof(struct Couche_Opacite));
        (*so)->opacite=o;
        (*so)->next = NULL;
    }
    else
    {
    Seau_Opacite nouveau_seau = NULL;
    ajout_o(o,&nouveau_seau);
    //This recursive call ends as nouveau_seau is NULL so the first recursive call will end up in the if condition
    nouveau_seau->next  = *so;
    *so = nouveau_seau;
    }
}

/*
@requires A Color Bucket
@ensures Removes the memory allocation for the first Color in the bucket
@returns The initial Color Bucket with the first Color removed
*/
Seau_Couleur Decapite_c(Seau_Couleur sc)
{
    Seau_Couleur new_seau = sc;
    sc = sc->next;
    free(new_seau);
    return sc;
}

/*
@requires An Opacity Bucket
@ensures Removes the memory allocation for the first Opacity in the bucket
@returns The initial Opacity Bucket with the first Opacity removed
*/
Seau_Opacite Decapite_o(Seau_Opacite so)
{
    Seau_Opacite new_seau = so;
    so = so->next;
    free(new_seau);
    return so;
}

/*
@requires A Color Bucket
@ensures Removes the memory allocation for the bucket
@returns Nothing
*/
void Vide_Seau_c(Seau_Couleur *sc)
{
    while (*sc != NULL)
    {
        *sc = Decapite_c(*sc);
    }
    //This while loop ends as the size of the bucket is strictly decreasing and the bucket has a finite size
}

/*
@requires An opacity Bucket
@ensures Removes the memory allocation for the bucket
@returns Nothing
*/
void Vide_Seau_o(Seau_Opacite *so)
{
    while (*so != NULL)
    {
        *so = Decapite_o(*so);
    }
    //This while loop ends as the size of the bucket is strictly decreasing and the bucket has a finite size
}

/*
@requires A Color Bucket
@ensures Computes the mean value of the Color in the bucket
@returns The computed mean Color
*/
Couleur moyenne_seau_c(Seau_Couleur sc)
{
    if (sc==NULL)
    {
        return creer_couleur(0,0,0);
    }
    int r = sc->couleur.rgb[0].n;
    int g = sc->couleur.rgb[1].n;
    int b = sc->couleur.rgb[2].n;
    int i = 1;
    Seau_Couleur new_seau = sc->next;
    while(new_seau != NULL)
    {
        r += new_seau->couleur.rgb[0].n;
        g += new_seau->couleur.rgb[1].n;
        b += new_seau->couleur.rgb[2].n;
        i+=1;
        new_seau = new_seau->next;
    }
    //This while loop ends as the linked list sc is finite and the iterator i is strictly increasing
    return creer_couleur(r/i,g/i,b/i);
}

/*
@requires An Opacity Bucket
@ensures Computes the mean value of Opacity in the Bucket
@returns The computed mean Opacity
*/
Opacite moyenne_seau_o(Seau_Opacite so)
{
    Opacite res;
    if (so == NULL)
    {
        res.alpha.n = 255;
        return res;
    }
    res = so->opacite;
    int i = 1;
    Seau_Opacite new_seau = so->next;
    while(new_seau != NULL)
    {
        res.alpha.n += new_seau->opacite.alpha.n;
        i+=1;
        new_seau = new_seau->next;
    }
    //This while loop ends as the linked list sc is finite and the iterator i is strictly increasing
    res.alpha.n = (res.alpha.n)/i;
    return res;

}

/*
@requires The Color and Opacity Buckets
@ensures Computes the value of the current Pixel using the mean value of the buckets
@returns The computed value of the current Pixel
*/
Pixel Calcul_Pixel_Courant(Seau_Couleur couleurs, Seau_Opacite opacites)
{
    Opacite op_cour = moyenne_seau_o(opacites);
    Couleur col_cour = moyenne_seau_c(couleurs);
    Pixel pix_cour;
    for (int i=0; i<3; i+=1)
    {
        pix_cour.couleur.rgb[i].n = (col_cour.rgb[i].n * op_cour.alpha.n)/255;
    }
    pix_cour.opacite = op_cour;
    return pix_cour;
}

/*
@requires A starting Pixel p0 : (x0,y0) and an ending Pixel p1 : (x1,x2), the current Pixel and the Calc stack
@ensures Paints a row composed of the current Pixel between p0 and p1
@returns Nothing
*/
void tracer_ligne(int x0, int y0, int x1, int y1, Pixel pix_cour, Calque calque_courant)
{
    int delta_x = x1-x0;
    int delta_y = y1-y0;
    int d = MAX(abs(delta_x), abs(delta_y));
    int s;
    if (delta_x*delta_y > 0)
    {
        s = 0;
    }
    else
    {
        s = 1;
    }
    int x = x0 * d + ((d - s)/2);
    int y = y0 * d + ((d - s)/2);
    for (int i=0; i<d; i+=1)
    {
        calque_courant.grille[x/d][y/d] = pix_cour;
        x+=delta_x;
        y+=delta_y;
    }
    calque_courant.grille[x1][y1] = pix_cour;
}



/*
@requires A stating Pixel p : (x,y), an old Pixel, a new Pixel and the current Calc
@ensures Fills the area of old Pixel in the current Calc with the new Pixel value starting at p
@returns Nothing
*/
void fill(int x, int y, Pixel ancien, Pixel nouveau, Calque calque)
{

    Position s = {x,y};
    Pile explicit_stack = NULL;
    push(s,&explicit_stack);
    int i = 0;
    while (explicit_stack != NULL )
    {
        Position d = pop(&explicit_stack);
        i-=1;
        if (!same_pixel(calque.grille[d.x][d.y], nouveau) && same_pixel(calque.grille[d.x][d.y], ancien))
        {
            calque.grille[d.x][d.y] = nouveau;
            if (d.x > 0)
            {
                Position a = {d.x-1,d.y};
                push(a, &explicit_stack);
                i+=1;
            }
            if (d.y > 0)
            {
                Position a = {d.x,d.y-1};
                push(a, &explicit_stack);
                i+=1;
            }
            if (d.x < size-1)
            {
                Position a = {d.x+1,d.y};
                push(a, &explicit_stack);
                i+=1;
            }
            if (d.y < size-1)
            {
                Position a = {d.x,d.y+1};
                push(a, &explicit_stack);
                i+=1;
            }
        }
    }
    //This loop ends because there is a finite number of position to add to the stack, at worse 4 times the image size
    //(1 value for each adjacent pixel of the image : 1 to the left, 1 to the right,...)
}

/*
@requires The Calc stack
@ensures Fuses the first two Calcs of the stack, if there is only one Calc, does nothing
@returns Nothing
*/
void fuse_calques(Calque c0, Calque c1)
{
    for (int i=0; i<size; i+=1)
    {
        for (int k=0; k<size; k+=1)
        {
            Opacite alpha0 = c0.grille[i][k].opacite;
            Couleur col0 = c0.grille[i][k].couleur;
            c1.grille[i][k].opacite.alpha.n = (c1.grille[i][k].opacite.alpha.n*(255 - alpha0.alpha.n))/255;
            c1.grille[i][k].opacite.alpha.n += alpha0.alpha.n;
            for (int j=0;j<3;j+=1)
            {
                c1.grille[i][k].couleur.rgb[j].n = (c1.grille[i][k].couleur.rgb[j].n*(255 - alpha0.alpha.n))/255;
                c1.grille[i][k].couleur.rgb[j].n += col0.rgb[j].n;
            }
        }
    }
}

/*
@requires The Calc stack
@ensures Cuts the second Calc of the stack using the first ones opacity, then removes the first calc. If there is only one Calc, does nothing
@returns Nothing
*/
void cut_calques(Calque c0, Calque c1)
{
    for (int i=0; i<size; i+=1)
    {
        for (int k=0; k<size; k+=1)
        {
            Opacite alpha0 = c0.grille[i][k].opacite;
            c1.grille[i][k].opacite.alpha.n = (c1.grille[i][k].opacite.alpha.n * alpha0.alpha.n)/255;
            for (int j=0;j<3;j+=1)
            {
                c1.grille[i][k].couleur.rgb[j].n = (c1.grille[i][k].couleur.rgb[j].n * alpha0.alpha.n)/255;
            }
        }
    }
}


int main(int argc, char** argv)
{
    if(argc!=3)
    {
        fprintf(stderr, "Usage : Input File, Output File\n");
    }

    FILE *fp = fopen(argv[1],"r");
    if (fp == NULL)
    {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier d'entrée\n");
        exit(2);
    }
    char buf[256];
    fgets(buf, 256, fp);
    sscanf(buf, "%d\n",&size);


    // Cursor position is initialized to (0,0)
    Position curseur;
    curseur.x = 0;
    curseur.y = 0;

    // The last marked position is initialized to (0,0)
    Position marquee;
    marquee.x = 0;
    marquee.y = 0;

    // Cursor direction if initialized to East
    char direction = 'E';

    // Buckets are initialized empty
    Seau_Couleur seau_couleur = NULL;
    Seau_Opacite seau_opacite = NULL;

    // Calc stack and first Calc initialized
    Pile_Calques calques = creer_pile_calques();
    empile_calque(&calques, creer_calque(size));

    // Usages variables initialized
    Couleur coul;
    Opacite op;
    Pixel pix_cour;

    char c = fgetc(fp);

    while (c != EOF )
    {
        switch (c)
        {
        case 'n':
            // Adds the color Black to the bucket
            coul = creer_couleur(0,0,0);
            ajout_c(coul, &seau_couleur);
            break;
        case 'r':
            // Adds the color Red to the bucket
            coul = creer_couleur(255,0,0);
            ajout_c(coul, &seau_couleur);
            break;
        case 'g':
            // Adds the color Green to the bucket
            coul = creer_couleur(0,255,0);
            ajout_c(coul, &seau_couleur);
            break;
        case 'b':
            // Adds the color Blue to the bucket
            coul = creer_couleur(0,0,255);
            ajout_c(coul, &seau_couleur);
            break;
        case 'y':
            // Adds the color Yellow to the bucket
            coul = creer_couleur(255,255,0);
            ajout_c(coul, &seau_couleur);
            break;
        case 'm':
            // Adds the color Magenta to the bucket
            coul = creer_couleur(255,0,255);
            ajout_c(coul, &seau_couleur);
            break;
        case 'c':
            // Adds the color Cyan to the bucket
            coul = creer_couleur(0,255,255);
            ajout_c(coul, &seau_couleur);
            break;
        case 'w':
            // Adds the color White to the bucket
            coul = creer_couleur(255,255,255);
            ajout_c(coul, &seau_couleur);
            break;
        case 't':
            // Adds the transparent opacity to the bucket
            op.alpha.n = 0;
            ajout_o(op, &seau_opacite);
            break;
        case 'o':
            // Adds the opaque opacity to the bucket
            op.alpha.n = 255;
            ajout_o(op, &seau_opacite);
            break;
        case 'i':
            // Empties both the Color and the Opacity buckets
            Vide_Seau_c(&seau_couleur);
            Vide_Seau_o(&seau_opacite);
            break;
        case 'v':
            // Moves the cursor 1 position in the current direction, if we cross the edge of the image returns at the beginning
            switch (direction)
            {
            case 'E':
                if (curseur.x == size-1)
                curseur.x = 0;
                else
                curseur.x += 1;
                break;
            case 'W':
                if (curseur.x == 0)
                curseur.x = size-1;
                else
                curseur.x -= 1;
                break;
            case 'N':
                if (curseur.y == 0)
                curseur.y = size-1;
                else
                curseur.y -= 1;
                break;
            case 'S':
                if (curseur.y == size-1)
                curseur.y = 0;
                else
                curseur.y += 1;
                break;
            default:
                break;
            }
            break;
        case 'h':
            // Rotates the current direction clockwise
            switch (direction)
            {
            case 'E':
                direction = 'S';
                break;
            case 'S':
                direction = 'W';
                break;
            case 'W':
                direction = 'N';
                break;
            case 'N':
                direction = 'E';
                break;
            default:
                break;
            }
            break;
        case 'a':
            // Rotates the current direction counter-clockwise
            switch (direction)
            {
            case 'W':
                direction = 'S';
                break;
            case 'N':
                direction = 'W';
                break;
            case 'E':
                direction = 'N';
                break;
            case 'S':
                direction = 'E';
                break;
            default:
                break;
            }
            break;
        case 'p':
            // Sets the marked position to the current cursor position
            marquee.x = curseur.x;
            marquee.y = curseur.y;
            break;
        case 'l':
            // Draws a line between the marked position and the current cursor position using the tracer_ligne function
            pix_cour = Calcul_Pixel_Courant(seau_couleur, seau_opacite);
            Calque calquel = pop_calque(&calques);
            tracer_ligne(marquee.x, marquee.y,curseur.x, curseur.y, pix_cour, calquel);
            empile_calque(&calques, calquel);
            break;
        case 'f':
            // Fills the area around the current cursor position using the fill function
            pix_cour = Calcul_Pixel_Courant(seau_couleur, seau_opacite);
            Calque calquef = pop_calque(&calques);
            fill(curseur.x, curseur.y, calquef.grille[curseur.x][curseur.y], pix_cour, calquef);
            empile_calque(&calques, calquef);
            break;
        case 's':
            // Adds a Calc to the Calc stack, with all color and opacity values for each pixel set to 0. If the Calc stack already has 10 Calcs, does nothing
            if (calques.top >= 9)
            {
                break;
            }
            Calque s = creer_calque(size);
            empile_calque(&calques, s);
            break;
        case 'e':
            // Fuses the first two Calcs in the Calc stack using the fuse_calques function. If the Calc stack only has one Calc, does nothing
            if (calques.top <= 0)
            {
                break;
            }
            Calque e0 = pop_calque(&calques);
            Calque e1 = pop_calque(&calques);
            fuse_calques(e0,e1);
            empile_calque(&calques, e1);
            break;
        case 'j':
            // Cuts the second Calc in the Calc stack using the opacity of the first Calc using the cut_calques function. If the Calc stack only has one Calc, does nothing
            if (calques.top <= 0)
            {
                break;
            }
            Calque c0 = pop_calque(&calques);
            Calque c1 = pop_calque(&calques);
            cut_calques(c0,c1);
            empile_calque(&calques, c1);
            break;
        default:
            break;
        }
        c = fgetc(fp);
    }
    // This while loop ends as the last character of the file is EOF and the file has a finite size

    fclose(fp);
    printf("My Work Here Is Done !\n");

    Calque ultimate_calque = pop_calque(&calques);

    Couleur* btm = calloc(size*size, sizeof(Couleur));
    for (int i = 0; i<size; i+=1)
    {
        for (int j = 0; j<size; j+=1)
        {
            btm[size*i+j]= ultimate_calque.grille[j][i].couleur;

        }
    }

    if (argc >= 3)
    {
        FILE* f;
        f = fopen (argv[2], "wb");
        if (f == NULL)
        {
            fprintf(stderr, "Erreur lors de l'ouverture du fichier de sortie\n");
            exit(2);
        }
        fprintf(f, "P6\n%d %d\n255\n",size, size);
        fwrite(btm,sizeof(Couleur),size*size,f);
        fclose(f);
    }
    else
    {
        fwrite(btm,sizeof(Couleur),size*size,stdout);
    }
    free(btm);

	return 0;

}
