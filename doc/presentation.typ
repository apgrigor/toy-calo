#let marg = 1 / 12
#let W = 480pt
#let H = 3 / 4 * W
#let content_width = W - 2.5 * W * marg

#set page(
    width: W,
    height: H,
    margin: (
        bottom: 0%,
        left: marg * 80%,
        right: marg * 80%,
        rest: marg * 100%
    )
)
#set text(
    font: "Liberation Serif",
    size: 11.5pt
)
#set par(
    justify: true
)
#set figure(
    supplement: [Σχήμα]
)

#let dark = purple.darken(50%)
#let light = orange.lighten(10%)
#let medium = purple.lighten(70%)
#let top_bar_width = 10%

#let mono(string) = text(string, font:"Liberation Mono", size:10pt)

#let tr_wedge(bot, fill) = place(
    top,
    polygon(
        fill: fill,
        (bot, 0%),
        (100%, 0%),
        (100%, top_bar_width),
        (bot + 5%, top_bar_width)
    )
)

#let top_bar = {
    place(
        top,
        polygon(
            fill: light,
            (0%, 0%),
            (100%, 0%),
            (100%, top_bar_width),
            (0%, top_bar_width)
        )
    )
    // tr_wedge(48%, light.darken(5%))
    tr_wedge(50%, medium)
    // tr_wedge(58%, medium.darken(5%))
    tr_wedge(60%, dark)
}


#let slide(title, content) = {
    pagebreak()
    place(
        top,
        dy: -5.5%,
        dx: -4%,
        text(
            size: 18pt,
            weight: "bold",
            fill: dark,
            title
        )
    )
    place(
        top + right,
        dy: -5.5%,
        dx: 4%,
        text(
            size: 18pt,
            fill: medium,
            counter(page).display("1")
        )
    )
    v(2cm)
    place(
        horizon,
        content
    )
}
#set align(center)
#v(10%)
#block(
    fill: light,
    inset: 12pt,
    radius: 4pt,
    text(
        size: 22pt,
        weight: "bold",
        fill: dark,
        "Απλοϊκό Μοντέλο Καλοριμέτρου: Διαχωρισμός Μεταξύ Ηλεκτρονίων/Φωτονίων και Αδρονίων"
    )
)

#v(4%)
#datetime.today().display("[day]/[month]/[year]")
#v(2%)

Γρηγοράς Απόλλων\
ΑΜ: 71101122000203\
email: #text("apollon@phys.uoa.gr")

#v(8%)
#image("../img/EKPA.png", width: 45%)

#counter(page).update(0)

#set align(left)
#set page(
    background: top_bar
)

#slide([Βασικό Μοντέλο])[
    #grid(
        columns: (54%, 46%),
        column-gutter: 0.4cm,
        figure(
            image("../img/theory/lattice.png"),
            caption: [Σχηματική αναπαράσταση του καλοριμέτρου στο σημείο πρόσπτωσης ενός σωματιδίου ($t=$ βάθος)]
        ), [
            Θεωρούμε το μέρος του καλοριμέτρου όπου προσπίπτει κάθε σωματίδιο, ανεξάρτητα από την γεωμετρία του ανιχνευτή.

            $9 times 9 times 9$ κύβος $(eta,phi.alt,t)$: Οι πρώτες 4 κυψελίδες κατά βάθος $t$ είναι μέρος του ΗΜ καλοριμέτρου και οι υπόλοιπες του Αδρονικού.

            Κάθε σωματίδιο εισέρχεται από το $(eta,phi.alt,t) = (4,4,0)$ με κατεύθυνση προς τα αυξανόμενα $t$.

            Δεν διαχωρίζουμε ανάμεσα στα δύο καλορίμετρα, αν και είναι διαφορετικού είδους.
        ]
    )
]

#slide([Διαμήκη Προφίλ #h(6pt)$e\/gamma$])[
    #grid(
        columns: (54%, 46%),
        column-gutter: 0.4cm,
        figure(
            image("../img/theory/egamma_profile.png"),
            caption: [Διαμήκη προφίλ εναπόθεσης ενέργειας ΗΜ σωματιδίων]
        ), [
            Η εναπόθεση ενέργειας προσεγγίζεται ως κατανομή Γ #cite("eg_profiles"):

            $
                f_e (t) 
                &= N_e L(t;alpha_e,beta_e)\
                &= N_e ((beta_e t)^(alpha_e - 1) e^(-beta_e t)) / Gamma(alpha_e)
            $

            Λογαριθμική εξάρτηση από την ενέργεια ($T$ = shower maximum):

            $
                T_e &= T_(e,0) + T_(e,1) ln E\
                alpha_e &= alpha_(e,0) + alpha_(e,1) ln E\
                beta_e &= (alpha_e - 1) / T_e 
            $

            Το $N_e$ επιλέγεται ώστε:

            $
                integral_0^(t_"max") f_e (t) dif t = 1
            $
        ]
    )
]


#slide([Διαμήκη Προφίλ Αδρονίων])[
    #grid(
        columns: (54%, 46%),
        column-gutter: 0.4cm,
        figure(
            image("../img/theory/hadron_profile.png"),
            caption: [Διαμήκη προφίλ εναπόθεσης ενέργειας αδρονίων]
        ), [
            Υπέρθεση δύο κατανομών Γ #cite("had_profiles"):

            $
                f_h (t) = N_h lr((
                    &c L(t; alpha_e, beta_e) +\
                    +&(1 - c) L(t; alpha_h, beta_h)
                ), size:#140%)
                    
            $

            Oμοίως με τα $e\/gamma$:

            $
                T_h &= T_(h,0) + T_(h,1) ln E\
                alpha_h &= alpha_(h,0) + alpha_(h,1) ln E\
                beta_h &= (alpha_h - 1) / T_h 
            $

            Και:

            $
                integral_0^(t_"max") f_h (t) dif t = 1
            $
        ]
    )
]

#slide([Εγκάρσια Προφίλ #h(6pt)$e\/gamma$])[
    #grid(
        columns: (54%, 46%),
        column-gutter: 0.4cm, [
            #figure(
                image("../img/theory/egamma_lat_profile.png"),
                caption: [Ακτίνα στην οποία περιέχεται το 95% της ενέργειας των ΗΜ σωματιδίων]
            ) <fig:lateg>
        ], [
            Εμπειρικό ακτινικό προφίλ #cite("gflash"):

            $
                g_e (r, t) = M_e (2 r R_(e,0)^2) / (r^2 + R_(e,0)^2)^2
            $

            Υποθέτουμε λογαριθμική εξάρτηση:

            $
                R_(e,0)(E, t) = R_(e,C) + (A_e ln E) t
            $

            Και $M_e (E, t)$ τέτοιο ώστε:

            $
                integral_0^(r_"max") g_e (r, t) dif r = 1
            $

            Αν δεν υπήρχε η $t$ εξάρτηση στο $M_e$, το @fig:lateg θα έδειχνε ευθείες.
        ]
    )
]

#slide([Εγκάρσια Προφίλ Αδρονίων])[
    #grid(
        columns: (54%, 46%),
        column-gutter: 0.4cm, [
            #figure(
                image("../img/theory/hadron_lat_profile.png"),
                caption: [Ακτίνα στην οποία περιέχεται το 95% της ενέργειας των αδρονίων]
            ) <fig:lathad>
        ], [
            Εμπειρικό ακτινικό προφίλ #cite("gflash"):

            $
                g_h (r, t) = M_h (2 r R_(h,0)^2) / (r^2 + R_(h,0)^2)^2
            $

            Διαφορεική εξάρτηση από την ενέργεια:

            $
                R_(h,0)(E, t) = (R_(h,C) + (A_h ln E) t)^2
            $

            Και:

            $
                integral_0^(r_"max") g_h (r, t) dif r = 1
            $
        ]
    )
]

#slide([Διακριτική Ικανότητα])[
    #grid(
        columns: (54%, 46%),
        column-gutter: 0.4cm, [
            #figure(
                image("../img/theory/energyVreso.png"),
                caption: [Διακριτική ικανότητα κάθε κελιού συναρτήσει της ενέργειας που εναποτίθεται σε αυτό]
            ) <fig:reso>
        ], [
            Η εναπόθεση ενέργειας $E$ επιφέρει γκαουσιανά κατανεμειμένο σφάλμα με τυπική απόκλιση #cite("ecal_tdr"):

            $
                sigma (Ε) = 
                    E sqrt((a / sqrt(E))^2 + (sigma_n / E)^2 + c^2)
            $

            Όπου:
            - $a\/sqrt(E)$ ο στοχαστικός όρος
            - $sigma_n\/E$ ο όρος θορύβου
            - $c$ ο σταθερός όρος

            Σχετικά ρεαλιστικές τιμές #cite("ecal_tdr"):

            $
                a &approx 5%\/sqrt("GeV")\
                sigma_n &approx 0.5 "GeV"\
                c &approx 0.55\%
            $ 
        ]
    )
]

#slide([Μορφή Της Προσομοίωσης])[
    Το πρόγραμμα χωρίζεται σε 3 βήματα προσομοίωσης και 1 βήμα διαχωρισμού:

    $
        mono("gen") 
        arrow.r.long mono("sim") 
        arrow.r.long mono("reco") 
        arrow.r.long mono("discr")
    $

    Πιο συγκεκριμένα:

    - #mono("gen"): Γέννηση των κινηματικών χαρακτηρηστικών των σωματιδίων (ενέργεια). Δεν διαχωρίζουμε ανάμεσα σε $e\/gamma$ και αδρόνια. Έχουμε 2 δείγματα:
        - #mono("flat"): Ομοιόμορφη κατανομή ενέργειας από 2 GeV ως 100 GeV.
        - #mono("exact"): Δημιουργία σωματιδίων με μόνο μία προκαθορισμένη ενέργεια.

    - #mono("sim"): Δειγματολειψία των προφίλ: δημιουργούνται $n$ σημεία, κάθε ένα εκ των οποίων αντιπροσωπεύει εναπόθεση ενέργειας $E\/n$ σε ένα κελί. Στη συνέχεια σε κάθε κελί προστίθεται μια γκασουσιανή συμβολή (gaussian smearing) με βάση την διακριτική ικανότητα του ανιχνευτή.

    - #mono("reco"): Μετατροπή των εναποθέσεων ενέργειας σε ολικές μεταβλητές:
        - #mono("energy"): Η συνολική ενέργεια μετά το gaussian smearing.
        - #mono("long_mean"): Μέση τιμή του βάθους, με βάρος την ενέργεια.
        - #mono("lat_mean"): Μέση τιμή της ακτίνας, με βάρος την ενέργεια.
]

#slide([#text("sim", font: "LiberationMono"): Πλέγμα (5 GeV)])[
    #figure(
        grid(
            columns: (50%, 50%),
            column-gutter: 0.4cm,
            image("../img/snap/flat_spec/e5/snap_03_mesh.png"),
            image("../img/snap/flat_spec/e5/snap_46_mesh.png"),
        ),
        caption: [Τυπικές ενεργειακές εναποθέσεις (αθροισμένες ως προς $phi.alt$) για σωματίδια αρχικής ενέργειας 5 GeV]
    )
]

#slide([#text("sim", font: "LiberationMono"): Πλέγμα (20 GeV)])[
    #figure(
        grid(
            columns: (50%, 50%),
            column-gutter: 0.4cm,
            image("../img/snap/flat_spec/e20/snap_12_mesh.png"),
            image("../img/snap/flat_spec/e20/snap_35_mesh.png"),
        ),
        caption: [Τυπικές ενεργειακές εναποθέσεις (αθροισμένες ως προς $phi.alt$) για σωματίδια αρχικής ενέργειας 20 GeV]
    )
]


#slide([#text("sim", font: "LiberationMono"): Πλέγμα (100 GeV)])[
    #figure(
        grid(
            columns: (50%, 50%),
            column-gutter: 0.4cm,
            image("../img/snap/flat_spec/e100/snap_13_mesh.png"),
            image("../img/snap/flat_spec/e100/snap_29_mesh.png"),
        ),
        caption: [Τυπικές ενεργειακές εναποθέσεις (αθροισμένες ως προς $phi.alt$) για σωματίδια αρχικής ενέργειας 100 GeV]
    )
]

#slide([#text("reco", font: "LiberationMono"): Γενικές Μεταβλητές])[
    #grid(
        columns: (50%, 50%),
        column-gutter: 0.4cm, [
            #figure(
                image("../img/comp/flat/energy.png"),
                caption: [Κατανομές ενέργειας μετά την "ανακατασκευή"]
            ) <fig:reco-energy>
        ], [
            Η αρχική ενέργεια και για τα δύο είδη σωματιδίων είναι ομοιόμορφα κατανεμημένη από 2 ως 100 GeV, όμως επειδή το smearing γίνεται ξεχωριστά σε κάθε κυψελίδα, επηρεάζεται από την κατανομή της ενέργειας στο πλέγμα.

            Οι τελικές ενέργειες έχουν διαφορετική κατανομή!
        ]
    )
]

#slide([#text("reco", font: "LiberationMono"): Γενικές Μεταβλητές])[
    #figure(
        grid(
            columns: (50%, 50%),
            column-gutter: 0.4cm,
            image("../img/comp/flat/long_mean.png"),
            image("../img/comp/flat/lat_mean.png"),
        ),
        caption: [Χωρικές κατανομές μετά την "ανακατασκευή"]
    )
]

#slide([#text("discr", font: "LiberationMono"): Διαχωρισμός ])[
    #grid(
        columns: (50%, 50%),
        column-gutter: 0.4cm, [
            #figure(
                image("../img/comp/flat/energyVlong.png"),
                caption: [Διαμήκης μέση τιμή συναρτήσει της ενέργειας]
            ) <fig:reco-scatter>
        ], [
            Ο διαχωρισμός του σήματος από το υπόβαθρο έγινε με δύο μεθόδους:
            - Απλές επιλογές (cuts)
            - Μηχανική Μάθηση (BDT/Boosted Decision Tree)

            Η συνολική ενέργεια και η διαμήκης μέση τιμή είναι αρκετές για πολύ καλό διαχωρισμό των σωματιδίων, αν ληφθούν υπ'όψη μαζί. Οι απλές επιλογές δεν είναι ικανές γι αυτό, όμως τα BDTs είναι.
        ]
    )
]

#slide([#text("discr", font: "LiberationMono"): Διαχωρισμός ])[
    Θεωρούμε τα $e\/gamma$ ως σήμα και τα αδρόνια ως υπόβαθρο.
    - Signal efficiency (απόδοση σήματος): Το ποσοστό του σήματος που έγινε δεκτό μετά τον διαχωρισμό.
    - Background efficiency (απόδοση υποβάθρου): Το ποσοστό του υποβάθρου που έγινε δεκτό μετά τον διαχωρισμό.

    Για την σύγκριση, επιλέξαμε cuts έτσι ώστε να δίνουν ίδια απόδοση σήματος με το BDT.

    #figure(
        table(
            columns:(auto, auto, auto),
            align: right,
            [method ], [cuts], [BDT],
            [signal efficiency], [0.992], [0.992],
            [background efficiency], [0.153], [0.029]
        ),
        supplement: [Πίνακας],
        caption: [Σύγκριση μεθόδων διαχωρισμού]
    )

    Παρατηρούμε $tilde 5$ φορές μείωση του υποβάθρου με το BDT. 
]

#slide([Διαχωρισμός και Ενέργεια])[
    #grid(
        columns: (50%, 50%),
        column-gutter: 0.4cm, [
            #figure(
                image("../img/discr/energyVeff.png"),
                caption: [Απόδοση υποβάθρου συναρτήσει της αρχικής ενέργειας με τη μέθοδο των απλών επιλογών, για δεδομένη απόδοση ]
            ) <fig:discr-energy>
        ], [
            Το @fig:discr-energy προήλθε από δείγματα στα οποία κάθε σωματίδιο είχε συγκεκριμένη αρχική ενέργεια, γι αυτό η απόρριψη του υποβάθρου είναι καλύτερη από δείγματα με σωματίδια πολλών αρχικών ενεργειών.

            Παρατηρείται πως ο διαχωρισμός είναι πολύ καλύτερος σε υψηλές ενέργειες, και μετά τα 10 GeV σχεδόν τέλειος.

            Χρησιμοποιήθηκε cut μόνο στη μεταβλητή #mono("long_mean"), υπολογίζοντας κάθε φορά την τιμή του ώστε η απόδοση σήματος να είναι σταθερή.
        ]
    )
]

#slide([Διακριτική Ικανότητα])[
    #grid(
        columns: (50%, 50%),
        column-gutter: 0.4cm, [
            #figure(
                image("../img/discr/resoVeff.png"),
                caption: [Απόδοση υποβάθρου συναρτήσει της διακριτικής ικανότητας, με τη μέθοδο των απλών επιλογών, για δεδομένη απόδοση ]
            ) <fig:discr-reso>
        ], [
            Το αναμενόμενο θα ήταν η απόρριψη του υποβάθρου να γίνεται χειρότερη όσο η διακριτική ικανότητα γίνεται χειρότερη, όμως παρατηρείται ένα ελάχιστο.
        ]
    )
]

#slide([Διακριτική Ικανότητα])[
    #figure(
        image("../img/comp/flat/long_mean_box.png")
    )

    Οι κατανομές μεταβάλλονται με τέτοιο τρόπο ώστε η επικάλυψη να ελαχιστοποιείται σε διακριτική ικανότητα $tilde 5$\@10GeV
]

#slide([Συμπεράσματα])[
    - Μέθοδοι μηχανικής μάθησης είναι ικανές να διακρίνουν ανάμεσα στο σήμα και το υπόβαθρο καλύτερα από απλές επιλογές.

    - Ο διαχωρισμός $e\/gamma$ από αδρόνια είναι πολύ καλός σε ενέργειες πάνω από 10 GeV.

    - Η διακριτική ικανότητα του ανιχνευτή επηρεάζει τα σωματίδια διαφορετικά ανάλογα με το προφίλ εναπόθεσης ενέργειάς τους.
]

#slide([Αναφορές])[
    #bibliography("bibliography.yml", title: [Βιβλιογραφία])

    = Κώδικας
    #link("https://github.com/apgrigor/toy-calo")
]