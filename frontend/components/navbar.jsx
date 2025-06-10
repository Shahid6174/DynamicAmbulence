'use client';
import React from 'react';
import { useState, useRef, useEffect } from 'react';
export default function Navbar() {
  const [open, setOpen] = useState(false);
  const dropdownRef = useRef(null);

  // Close dropdown on outside click
  useEffect(() => {
    const handleClickOutside = (event) => {
      if (dropdownRef.current && !dropdownRef.current.contains(event.target)) {
        setOpen(false);
      }
    };
    document.addEventListener('mousedown', handleClickOutside);
    return () => document.removeEventListener('mousedown', handleClickOutside);
  }, []);
  return (
    <nav className="navbar navbar-expand-lg bg-white border-bottom shadow-sm">
      <div className="container-fluid px-4">
        <a className="navbar-brand fw-bold text-primary d-flex align-items-center" href="/">
          <i className="bi bi-heart-pulse-fill me-2"></i> HealthApp
        </a>

        <button
          className="navbar-toggler"
          type="button"
          data-bs-toggle="collapse"
          data-bs-target="#topNavbar"
          aria-controls="topNavbar"
          aria-expanded="false"
          aria-label="Toggle navigation"
        >
          <span className="navbar-toggler-icon"></span>
        </button>

        <div className="collapse navbar-collapse" id="topNavbar">
          <ul className="navbar-nav ms-auto">
            <li className="nav-item dropdown" ref={dropdownRef}>
              <button
                onClick={() => setOpen(!open)}
                className="btn nav-link dropdown-toggle d-flex align-items-center text-dark bg-transparent border-0"
              >
                <i className="bi bi-person-circle fs-5 me-2"></i> Admin
              </button>

              {open && (
                <ul className="dropdown-menu dropdown-menu-end show mt-2 shadow-sm" style={{ minWidth: '12rem' }}>
                  <li>
                    <a className="dropdown-item d-flex align-items-center" href="/settings">
                      <i className="bi bi-gear me-2 text-secondary"></i> Profile
                    </a>
                  </li>
                  <li><hr className="dropdown-divider" /></li>
                  <li>
                    <a className="dropdown-item d-flex align-items-center text-danger" href="/logout">
                      <i className="bi bi-box-arrow-right me-2"></i> Logout
                    </a>
                  </li>
                </ul>
              )}
            </li>
          </ul>
        </div>
      </div>

      <style jsx>{`
        .dropdown-menu {
          border-radius: 0.5rem;
          border: 1px solid #dee2e6;
        }
        .dropdown-item:hover {
          background-color: #f8f9fa;
        }
        .dropdown-toggle::after {
          display: none;
        }
      `}</style>
    </nav>
  );
}
