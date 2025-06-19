"use client"
import React from 'react';
import TopNav from '../../../components/navbar.jsx';
import Sidebar from './sidebar.jsx';
import { useState } from "react"

import Stats from "./(subPages)/stats.jsx"
import Patients from "./(subPages)/patients.jsx"
import Hospitals from "./(subPages)/hospitals.jsx"

import Distress from "./(subPages)/distress.jsx"
import RatingHospital from "./(subPages)/rating_hospital.jsx"
import UserControl from "./(subPages)/usercontrol.jsx"

export default function MainApp (props){
  const [section, setSection] = useState(0)
  return (
    <div id="mainApp" className="page" >
      <TopNav />
      <div className="container-fluid">
        <div className="row">
          <Sidebar section = {section} setSection = {setSection}/>
          <main className="col-md-9 ms-sm-auto col-lg-10 px-md-4 py-4">
            <div id="dashboardPage" className="content-page">
              <h1 className="h2 text-primary mb-4">Welcome back, Admin!</h1>
              <div className="row g-4" id="statsCards">

              </div>

                {section === 0 && <Stats />}
                {section === 1 && <Patients/>}
                {section === 2 && < Hospitals />}
                {section === 3 && < Distress/>}
                {section === 4 && < RatingHospital />}
                {section === 5 && < UserControl />}

            </div>
          </main>
        </div>
      </div>
    </div>
  );
};


