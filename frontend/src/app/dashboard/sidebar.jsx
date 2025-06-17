'use client';
import {useState} from "react"


export default function Sidebar({section,setSection}) {
   



  const menuItems = [
    { icon: 'bi-speedometer2', text: 'Dashboard', route: '#dashboard' },
    { icon: 'bi-people-fill', text: 'Patients', route: '#patients' },
    { icon: 'bi-calendar-check-fill', text: 'Appointments', route: '#appointments' },
    { icon: 'bi-person-badge-fill', text: 'Doctors', route: '#doctors' },
    { icon: 'bi-file-medical-fill', text: 'Reports', route: '#reports' },
    { icon: 'bi-receipt-cutoff', text: 'Billing', route: '#billing' },
    { icon: 'bi-gear-fill', text: 'Settings', route: '#settings' },
  ];

  return (
    <aside id="sidebar" className="bg-light vh-100 shadow-sm" style={{ width: '200px' }}>
      <nav className="p-2">
        <ul className="nav flex-column">
          {menuItems.map((item, index) => (
            <li key={index} className="nav-item mb-2">
              <p onClick = {() => setSection(index)}
                className="nav-link d-flex align-items-center text-dark fw-medium rounded px-3 py-2 hover-bg"
               style={ (section ==index ) ?  {backgroundColor:"#5caaff" , boxShadow: "0 0 2px #5caaff, 0 0 2px #5caaff, 0 0 2px #5caaff",}:{ cursor: 'pointer' }}
              >
                <i className={`bi ${item.icon} me-2`}></i>
                {item.text}
              </p>
            </li>
          ))}
        </ul>
      </nav>

      {/* Custom hover styling */}
      <style jsx>{`
        .hover-bg:hover {
          background-color: #5caaff;
          text-decoration: none;
        }
      `}</style>
    </aside>
  );
}
