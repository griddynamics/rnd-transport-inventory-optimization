#include <stdio.h>
#include <assert.h>

#include "funcs.h"
#include "ortools.h"
#include "listarr2d.h"
#include "listarr3d.h"


static void fill_var_name(
    char* buff, int buff_size, int i, int idx_dc, int idx_carrier)
{
    snprintf(buff, buff_size, "x_%03d__%02d_%02d", i, idx_dc, idx_carrier); 
    return;
};


void ort_mk_header(FILE* f, const char* indent)
{
    const char* strs[] = {
        "import sys",
        "import os",
        "from ortools.linear_solver import pywraplp",
        "", 
        "# Create the mip solver with the SCIP backend:",
        "solver = pywraplp.Solver.CreateSolver('SCIP')",
        "if not solver:",
        "    print(' Error-1')",
        "    sys.exit()" 
    };
    int nstrs = 9;
    for(int i=0; i<nstrs; i++)
        fprintf(f, "%s%s\n", indent, strs[i]);
}



//  Variables i,j
void ort_mk_binvars(FILE* f, const prb* p, const char* indent)
{
    assert(prb_is_initialized(p));
    int nrows = p->nrows;
    int ncols = p->ncols;
    // ensure a single route for each carrier
    assert(p->ncols == p->ndc * p->ncarriers);  

    fprintf(f, "\n");
    fprintf(f, "#####  Step: Define binary variables\n");
    fprintf(f, "\n");

    //  Declare integer binary variables x(i,j)  (x(i,j)) 
    for(int i=0; i<nrows; i++)
        for(int j=0; j<ncols; j++)
        {
            char name[128];  // 
            int idx_dc = arr1d_getval(&(p->dc), j);
            int idx_carrier = arr1d_getval(&(p->carrier), j);

            // create the name of x(i,j):
            fill_var_name(name, 127, i, idx_dc, idx_carrier);

            // code: define x(i,j) 
            if(arr2d_getval(&(p->var_cost), i, j) >= 0) 
                fprintf(
                    f, 
                    "%s = solver.IntVar(0.0, 1.0, '%s')\n", 
                    name, name);
            else
                fprintf(f, "    # %s no such a variable \n", name);

        }

    fprintf(
        f, 
        "print(f'Number of binary variables = {solver.NumVariables()}')\n");

};

//  Consraints rowsum = 1 (a single option in a row) 
void ort_mk_constr_rowsum(FILE* f, const prb* p, const char* indent)
{
    assert(prb_is_initialized(p));
    int nrows = p->nrows;
    int ncols = p->ncols;
    assert(p->ncols == p->ndc * p->ncarriers);  // ensure a single route 
                                               //    for each carrier
    fprintf(f, "\n");
    fprintf(f, "#####  Step: Define constraints rowsum==1");
    fprintf(f, "\n");

    for(int i=0; i<nrows; i++)
    {
        fprintf(f, "\n# rowsum(%d) == 1\n", i);
        fprintf(f, "solver.Add(\n");

        int count = 0;
        for(int j=0; j<ncols; j++)
        {
            if(arr2d_getval(&(p->var_cost), i, j) < 0)   // no such an option 
                continue;

            char name[128];  // 
            int idx_dc = arr1d_getval(&(p->dc), j);
            int idx_carrier = arr1d_getval(&(p->carrier), j);


            fill_var_name(name, 127, i, idx_dc, idx_carrier);

            if(count > 0)
                fprintf(f, "%s+ %s\n", indent, name);
            else
                fprintf(f, "%s%s\n", indent, name);

            count += 1;
        }

        // close "solver.Add("
        fprintf(
            f, "%s== 1.0)  # End rowsum==1 for row %d\n",  indent, i); 

    }; // for(i=0; i<nrows; i++)

};

// The Objective 
void ort_mk_constr_objective(FILE* f, const prb* p, const char* indent)
{
    assert(prb_is_initialized(p));
    int nrows = p->nrows;
    int ncols = p->ncols;
    assert(p->ncols == p->ndc * p->ncarriers);  // ensure a single route 
                                               //    for each carrier
    fprintf(f, "\n");
    fprintf(f, "#####  Step 3: Define the objective");
    fprintf(f, "\n");


    fprintf(f, "solver.Minimize(\n");
    int count = 0;
    for(int i=0; i<nrows; i++)
    {
        // fprintf(f, "\n%s# rowsum(%d) == 1\n", indent, i);
        for(int j=0; j<ncols; j++)
        {
            if(arr2d_getval(&(p->var_cost), i, j) < 0)   // no such an option 
                continue;

            char name[128];  // 
            int idx_dc = arr1d_getval(&(p->dc), j);
            int idx_carrier = arr1d_getval(&(p->carrier), j);

            int var_cost_ij = arr2d_getval(&(p->var_cost), i, j);
            assert(var_cost_ij >= 0);
            int cost_ij = var_cost_ij * arr1d_getval(&(p->row_volume), i);

            fill_var_name(name, 127, i, idx_dc, idx_carrier);

            const char* eol;
            if(j < ncols-1)
                eol = "\n";
            else
                eol = "\n";

            if(count > 0)
                fprintf(f, "%s+ %d * %s %s", indent, cost_ij, name, eol);
            else
                fprintf(f, "%s%d * %s %s", indent, cost_ij, name, eol);

            count += 1;
        }

    }; // for(i=0; i<nrows; i++)

    // close "solver.Minimize("
    fprintf(f, ")\n"); 
    fprintf(
        f, 
        "\nprint(f'Number of constraints = {solver.NumConstraints()}')\n");


};

typedef struct
{
    prb* p;
    FILE* f;
} procpar;

//+++++++++++++++++++++++++++++++++++++++++++++++++
//static void proc_dc_sday(int i, int j, void* par, int item_idx)
static void proc_ij_var(int i, int j, void* par, int item_idx)
//+++++++++++++++++++++++++++++++++++++++++++++++++
{
    procpar* proc_par = (procpar*)par;
    prb* p = proc_par->p;
    FILE* f = proc_par->f;
    const char* indent = "    ";

    char name[128];  // 
    int dc = arr1d_getval(&(p->dc), j);
    int carrier = arr1d_getval(&(p->carrier), j);

    // create the name of x(i,j):
    fill_var_name(name, 127, i, dc, carrier);
    int volume_i = arr1d_getval(&(p->row_volume), i);

    if(item_idx > 0)
        fprintf(f, "%s+ %d * %s\n", indent, volume_i, name);
    else
        fprintf(f, "%s%d * %s\n", indent, volume_i, name);
};


//=====================================================================
void ort_mk_constr_dc_sday(FILE* f, const prb* p, const char* indent)
//=====================================================================
{
    larr2d la = larr2d_getnew(p->ndc, p->ndays);

    procpar proc_par;
    proc_par.f = f;
    proc_par.p = (prb*)p;

    fprintf(f, "\n");
    fprintf(f, "#####  Step: Define constraints dc_sday");
    fprintf(f, "\n");


    // Fill lists by (i,j) pairs
    for(int i=0; i<p->nrows; i++)
        for(int j=0; j<p->ncols; j++)
        {
            if(! prb_is_valid_ij(p, i, j))
                continue;

            // Get "dc" and "sday":
            int dc = arr1d_getval(&(p->dc), j); 
            int duration = arr1d_getval(&(p->duration), j); 
            int dday = arr1d_getval(&(p->row_dday), i);
            int sday = day(dday - duration, p->ndays);

            // Just other varnames (as in listarr2d)
            int k = dc;
            int l = sday;

            // Add [append] (i,j) to the (k,l)-th list:
            larr2d_add_kl(la, k, l,  i, j);
        };

    for(int dc=0; dc<p->ndc; dc++)
        for(int sday=0; sday<p->ndays; sday++)
        {
            // Just other varnames (as in listarr2d)
            int k = dc;
            int l = sday;

            if(larr2d_nitems_kl(la, k, l) == 0)
                continue;
           
            fprintf(f, "solver.Add(\n");

            larr2d_process_kl(la, k, l,  &proc_ij_var, (void*)&proc_par);

            int max_constr_kl = arr2d_getval(&(p->c_max.dc_sday), dc, sday);
            fprintf(
                f, 
                "%s<=%d) # max for DC= %d sday= %d\n", 
               indent, max_constr_kl, dc, sday);

        };

    larr2d_release(la);
}


//===========================================================================
void ort_mk_constr_carrier_dc_sday(FILE* f, const prb* p, const char* indent)
//===========================================================================
{
    larr3d la = larr3d_getnew(p->ncarriers, p->ndc, p->ndays);

    procpar proc_par;
    proc_par.f = f;
    proc_par.p = (prb*)p;

    fprintf(f, "\n");
    fprintf(f, "#####  Step: Define constraints carrier_dc_sday");
    fprintf(f, "\n");


    // Fill lists by (i,j) pairs
    for(int i=0; i<p->nrows; i++)
        for(int j=0; j<p->ncols; j++)
        {
            if(! prb_is_valid_ij(p, i, j))
                continue;

            // Get "carrier", "dc", and "sday":
            int carrier = arr1d_getval(&(p->carrier), j); 
            int dc = arr1d_getval(&(p->dc), j); 
            int dday = arr1d_getval(&(p->row_dday), i);
            //
            int duration = arr1d_getval(&(p->duration), j); 
            int sday = day(dday - duration, p->ndays);

            // Just other varnames (as in listarr3d)
            int k = carrier;
            int l = dc;
            int m = sday;

            // Add [append] (i,j) to the (k,l,m)-th list:
            larr3d_add_klm(la, k, l, m, i, j);
        };

    for(int carrier=0; carrier<p->ncarriers; carrier++)
        for(int dc=0; dc<p->ndc; dc++)
            for(int sday=0; sday<p->ndays; sday++)
            {
                // Just other varnames (as in listarr3d)
                int k = carrier;
                int l = dc;
                int m = sday;
                //
                if(larr3d_nitems_klm(la, k, l, m) == 0)
                    continue;
               
                fprintf(f, "solver.Add(\n");

                larr3d_process_klm(la, k, l, m, &proc_ij_var, (void*)&proc_par);

                int max_constr_klm = arr3d_getval(
                    &(p->c_max.carrier_dc_sday), carrier, dc, sday);
                fprintf(
                    f, 
                    "%s<=%d) # max for carrier= %d DC= %d sday= %d\n", 
                   indent, max_constr_klm, carrier, dc, sday);

            };

    larr3d_release(la);

}


void ort_solution(FILE* f, const prb* p)
{
    assert(prb_is_initialized(p));
    int nrows = p->nrows;
    int ncols = p->ncols;
    assert(p->ncols == p->ndc * p->ncarriers);  // ensure a single route 
                                                //    for each carrier
    fprintf(f, "\n");
    fprintf(f, "#####  Step Last: Solve and print the solution\n");
    fprintf(f, "\n");

    
    fprintf(f, "\n");
    fprintf(f, "#  Call the solver\n");
    fprintf(f, "status = solver.Solve()\n");
    fprintf(f, "\n");
    fprintf(f, "\n");

    fprintf(f, "if status != pywraplp.Solver.OPTIMAL:\n");
    fprintf(f, "    print('No solution found')\n");
    fprintf(f, "    exit()\n");
    fprintf(f, "\n");
    fprintf(f, "#  Print the solution:\n");
    fprintf(f, "\n");
     

    for(int i=0; i<nrows; i++)
    {
        fprintf(f, "\n");
        fprintf(f, "print('\\n')\n");
        for(int j=0; j<ncols; j++)
        {
            if(arr2d_getval(&(p->var_cost), i, j) < 0)   // no such an option 
                continue;

            char name[128];  // 
            int idx_dc = arr1d_getval(&(p->dc), j);
            int idx_carrier = arr1d_getval(&(p->carrier), j);
            int var_cost_ij = arr2d_getval(&(p->var_cost), i, j);
            assert(var_cost_ij >= 0);
            //
            fill_var_name(name, 127, i, idx_dc, idx_carrier);

            fprintf(f, "print('  %s =', %s.solution_value())\n", name, name);

        }  // for(int j=0; j<ncols; j++)
    }; // for(i=0; i<nrows; i++)

    fprintf(f, "\n");
    fprintf(f, "print('\\n')\n");
    fprintf(f, "print('Objective value =', solver.Objective().Value())\n");

};
